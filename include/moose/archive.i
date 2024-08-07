// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <moose/archive.h>
#include <moose/exceptions.h>
#include <moose/serialize.h>
#include <moose/type_traits.h>
#include <moose/types.h>
#include <cassert>

namespace moose::detail
{
  template <class T, bool isPointer>
  struct InitialValue;

  template <class T>
  struct InitialValue <T, false>
  {
    static T value () {return T {};}
  };

  template <class T>
  struct InitialValue <T, true>
  {
    static T value () {return nullptr;}
  };

  template <class T>
  T GetInitialValue ()
  {
    return InitialValue <T, std::is_pointer <T>::value>::value ();
  }

  template <class T>
  Hint hintOrDefault (T const& t, Hint hint)
  {
    if (hint != Hint::None)
      return hint;
    return getDefaultHint (t);
  }
}// end of namespace

namespace moose
{
  template <class T>
  void Archive::operator () (const char* name, T& value, Hint hint)
  {
    static constexpr EntryType entryType = TypeTraits <T>::entryType;
    auto const contentType = this->contentType (TypeTraits <T> {}, EntryTypeDummy<entryType> {});

    if (!begin_entry (name, contentType, detail::hintOrDefault (value, hint)))
      throw ArchiveError () << "No entry with name '" << name
        << "' found in current object '" << name << "'.";
    archive (name, value, EntryTypeDummy <entryType> ());
    end_entry (name, contentType);
  }

  template <class T>
  void Archive::operator () (const char* name, T& value, const T& defVal, Hint hint)
  {
    static constexpr EntryType entryType = TypeTraits <T>::entryType;
    auto const contentType = this->contentType (TypeTraits <T> {}, EntryTypeDummy<entryType> {});

    if (!begin_entry (name, contentType, detail::hintOrDefault (value, hint)))
    {
      value = defVal;
      return;
    }

    try
    {
      archive (name, value, EntryTypeDummy <entryType> ());
    }
    catch(ArchiveError&)
    {
      value = defVal;
    }

    end_entry (name, contentType);
  }

  template <class T>
  void Archive::operator () (const char* name, T const& value, Hint hint)
  {
    if (!is_writing ())
    {
      assert (!"Const values may only be passed to writing archives.");
      return;
    }

    (*this) (name, const_cast<T&> (value), hint);
  }

  template <class T>
  void Archive::operator () (const char* name, T const& value, const T& defVal, Hint hint)
  {
    if (!is_writing ())
    {
      assert (!"Const values may only be passed to writing archives.");
      return;
    }

    (*this) (name, const_cast<T&> (value), defVal, hint);
  }

  template <class T>
  Type const& Archive::archive_type (T& instance)
  {
    if (is_writing ())
    {
      auto const& type = types ().get_polymorphic (instance);
      mOutput->write_type_name (type.name ());
      return type;
    }
    else
    {
      auto const& typeName = mInput->type_name ();
      if (typeName.empty ())
        return types ().get <T> ();
      return types ().get (typeName);
    }
  }

  template <class T>
  void Archive::archive (const char* name, T& value, EntryTypeDummy <EntryType::Value>)
  {
    archive (name, value);
  }

  template <class T>
  void Archive::archive (const char* /*name*/, T& value, EntryTypeDummy <EntryType::Struct>)
  {
    Serialize (*this, value);
  }

  template <class T>
  void Archive::archive (const char* name, std::shared_ptr<T>& sp, EntryTypeDummy <EntryType::Struct>)
  {
    Type const& type = archive_type <T> (*sp);
    if(sp == nullptr)
    {
      if (is_reading ())
        sp = type.make_shared <T> ();
      else
        throw ArchiveError () << "JSONArchive::write cannot serialize nullptr: '" << name << "'";
    }

    type.serialize (*this, *sp);
  }

  template <class T>
  void Archive::archive (const char* name, std::unique_ptr<T>& up, EntryTypeDummy <EntryType::Struct>)
  {
    Type const& type = archive_type <T> (*up);
    if(up == nullptr)
    {
      if (is_reading ())
        up = type.make_unique <T> ();
      else
        throw ArchiveError () << "JSONArchive::write cannot serialize nullptr: '" << name << "'";
    }

    type.serialize (*this, *up);
  }

  template <class T>
  void Archive::archive (const char* name, T*& p, EntryTypeDummy <EntryType::Struct>)
  {
    Type const& type = archive_type <T> (*p );
    if(p ==  nullptr)
    {
      if (is_reading ())
        p = type.make_raw <T> ();
      else
        throw ArchiveError () << "JSONArchive::write cannot serialize nullptr: '" << name << "'";
    }

    type.serialize (*this, *p);
  }

  template <class T>
  void Archive::archive (const char* name, T& value, EntryTypeDummy <EntryType::Vector>)
  {
    using Traits = TypeTraits<T>;
    using ValueType = typename Traits::ValueType;

    constexpr bool unpack = wantsToUnpack<T> () && canBeUnpacked<ValueType> ();

    if (is_reading ())
    {
      Traits::clear (value);
      if constexpr (unpack)
      {
        while(mInput->array_has_next (name))
        {
          ValueType childValue;
          auto childRange = TypeTraits<ValueType>::toRange (childValue);
          for (auto i = childRange.begin; i != childRange.end; ++i)
          {
            if (!mInput->array_has_next (name))
              throw ArchiveError () << "Too few entries while reading range '" << name << "'";

            (*this) ("", *i);
          }
          Traits::pushBack (value, childValue);
        }
      }
      else
      {
        while(mInput->array_has_next (name))
        {
          ValueType tmpVal = detail::GetInitialValue <ValueType> ();
          (*this) ("", tmpVal);
          Traits::pushBack (value, tmpVal);
        }
      }
    }
    else
    {
      if constexpr (unpack)
      {
        auto range = Traits::toRange (value);
        for (auto i = range.begin; i != range.end; ++i)
        {
          auto childRange = TypeTraits<ValueType>::toRange (*i);
          for (auto j = childRange.begin; j != childRange.end; ++j)
            (*this) ("", *j);
        }
      }
      else
      {
        // Writing a vector type is the same as writing a range
        archive (name, value, EntryTypeDummy <EntryType::Range> ());
      }
    }
  }

  template <class T>
  void Archive::archive (const char* name, T& value, EntryTypeDummy <EntryType::Range>)
  {
    auto const range = TypeTraits<T>::toRange (value);
    if (is_reading ())
    {
      for (auto i = range.begin; i != range.end; ++i)
      {
        if (!mInput->array_has_next (name))
          throw ArchiveError () << "Too few entries while reading range '" << name << "'";

        (*this) ("", *i);
      }

      if (mInput->array_has_next (name))
        throw ArchiveError () << "Too many entries while reading range '" << name << "'";
    }
    else
    {
      for (auto i = range.begin; i != range.end; ++i)
        (*this) ("", *i);
    }
  }

  template <class T>
  void Archive::archive (const char* name, T& value, EntryTypeDummy <EntryType::ForwardValue>)
  {
    using ForwardedType = typename TypeTraits<T>::ForwardedType;
    auto constexpr forwardedEntryType = TypeTraits<ForwardedType>::entryType;

    if (is_reading ())
    {
      ForwardedType t;
      archive (name, t, EntryTypeDummy <forwardedEntryType> ());
      TypeTraits<T>::setForwardedValue (value, std::move (t));
    }
    else
    {
      auto tmp = TypeTraits<T>::getForwardedValue (value);
      archive (name, tmp, EntryTypeDummy<forwardedEntryType> ());
    }
  }

  template <class T>
  void Archive::archive (const char* name, T& value, EntryTypeDummy <EntryType::ForwardReference>)
  {
    using ForwardedType = typename TypeTraits<T>::ForwardedType;
    auto constexpr forwardedEntryType = TypeTraits<ForwardedType>::entryType;

    if (is_reading ())
    {
      ForwardedType t;
      archive (name, t, EntryTypeDummy <forwardedEntryType> ());
      TypeTraits<T>::setForwardedValue (value, std::move (t));
    }
    else
    {
      archive (
          name,
          const_cast<ForwardedType&> (TypeTraits<T>::getForwardedValue (value)),
          EntryTypeDummy<forwardedEntryType> ());
    }
  }

  template <class T>
  void Archive::archive (const char* name, T& value)
  {
    if (is_reading ())
      mInput->read (name, value);
    else
      mOutput->write (name, value);
  }

  template <class TRAITS>
  auto Archive::contentType (TRAITS const&, EntryTypeDummy<EntryType::Struct>) -> ContentType
  {
    return ContentType::Struct;
  }

  template <class TRAITS>
  auto Archive::contentType (TRAITS const&, EntryTypeDummy<EntryType::Value>) -> ContentType
  {
    return ContentType::Value;
  }

  template <class TRAITS>
  auto Archive::contentType (TRAITS const&, EntryTypeDummy<EntryType::Range>) -> ContentType
  {
    return ContentType::Array;
  }

  template <class TRAITS>
  auto Archive::contentType (TRAITS const&, EntryTypeDummy<EntryType::Vector>) -> ContentType
  {
    return ContentType::Array;
  }

  template <class TRAITS>
  auto Archive::contentType (TRAITS const&, EntryTypeDummy<EntryType::ForwardValue>) -> ContentType
  {
    using ForwardTraits = TypeTraits<typename TRAITS::ForwardedType>;
    return contentType (ForwardTraits {}, EntryTypeDummy<ForwardTraits::entryType> {});
  }

  template <class TRAITS>
  auto Archive::contentType (TRAITS const&, EntryTypeDummy<EntryType::ForwardReference>) -> ContentType
  {
    using ForwardTraits = TypeTraits<typename TRAITS::ForwardedType>;
    return contentType (ForwardTraits {}, EntryTypeDummy<ForwardTraits::entryType> {});
  }
}// end of namespace moose
