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
#include <moose/type_traits.h>

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
}// end of namespace

namespace moose
{
  template <class T>
  void Archive::operator () (const char* name, T& value)
  {
    static constexpr EntryType entryType = TypeTraits <T>::entryType;

    begin_entry (name, entryType);
    archive (name, value, EntryTypeDummy <entryType> ());
    end_entry (name, entryType);
  }

  template <class T>
  void Archive::operator () (const char* name, T& value, const T& defVal)
  {
    static constexpr EntryType entryType = TypeTraits <T>::entryType;

    try
    {
      begin_entry (name, entryType);
    }
    catch(ArchiveError&)
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

    end_entry (name, entryType);
  }

  template <class T>
  Type const& Archive::archive_type (T& instance)
  {
    if (is_writing ())
    {
      auto const& type = Types::get_polymorphic (instance);
      write_type_name (type.name ());
      return type;
    }
    else
    {
      auto const& typeName = read_type_name ();
      if (typeName.empty ())
        return Types::get <T> ();
      return Types::get (typeName);
    }
  }

  template <class T>
  void Archive::archive (const char* name, T& value, EntryTypeDummy <EntryType::Value>)
  {
    archive (name, value);
  }

  template <class T>
  void Archive::archive (const char* name, T& value, EntryTypeDummy <EntryType::Struct>)
  {
  ///todo: check for POD types and raise an error (->unsupported POD type)
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
    if (is_reading ())
    {
      while(read_array_has_next (name))
      {
        using ValueType = typename VectorTraits <T>::ValueType;
        ValueType tmpVal = detail::GetInitialValue <ValueType> ();
        (*this) ("", tmpVal);
        VectorPushBack (value, tmpVal);
      }
    }
    else
    {
      // Writing a vector type is the same as writing a range
      archive (name, value, EntryTypeDummy <EntryType::Range> ());
    }
  }

  template <class T>
  void Archive::archive (const char* name, T& value, EntryTypeDummy <EntryType::Range>)
  {
    auto range = make_range (value);
    if (is_reading ())
    {
      for (auto i = range.begin; i != range.end; ++i)
      {
        if (!read_array_has_next (name))
          throw ArchiveError () << "Too few entries while reading range '" << name << "'";

        (*this) ("", *i);
      }

      if (read_array_has_next (name))
        throw ArchiveError () << "Too many entries while reading range '" << name << "'";
    }
    else
    {
      for (auto i = range.begin; i != range.end; ++i)
        (*this) ("", *i);
    }
  }
}// end of namespace moose