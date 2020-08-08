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

template <class T, bool rangeSerialization>
struct ValueOrRange;

template <class T>
struct ValueOrRange <T, false>
{
  using type_t = T&;
  static type_t value (T& t) {return t;}
};

template <class T>
struct ValueOrRange <T, true>
{
  using type_t = decltype (make_range (T{}));
  static type_t value (T& t) {return make_range (t);}
};

template <class T>
auto GetValueOrRange (T& value)
-> typename ValueOrRange <T, RangeSerialization <T>::enabled>::type_t
{
  return ValueOrRange <T, RangeSerialization <T>::enabled>::value (value);
}

}// end of namespace

namespace moose
{

template <class T>
void Archive::operator () (const char* name, T& value)
{
  begin_archive (name);
  archive (name, detail::GetValueOrRange (value));
  end_archive (name);
}

template <class T>
void Archive::operator () (const char* name, T& value, const T& defVal)
{
  try
  {
    begin_read (name);
  }
  catch(ArchiveError&)
  {
    value = defVal;
    return;
  }

  try
  {
    read (name, detail::GetValueOrRange (value));
  }
  catch(ArchiveError&)
  {
    value = defVal;
  }

  end_read (name);
}

template <class T>
Type const& Archive::concrete_type ()
{
  auto const typeName = get_type_name ();
  if (typeName.empty ())
    return Types::get <T> ();
  return Types::get (typeName);
}

template <class T>
void Archive::archive (const char* name, T& value)
{
///todo: check for POD types and raise an error (->unsupported POD type)
  Serialize (*this, value);
}

template <class T>
void Archive::archive (const char* name, std::shared_ptr<T>& sp)
{
  Type const& type = concrete_type <T> ();
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
void Archive::archive (const char* name, std::unique_ptr<T>& up)
{
  Type const& type = concrete_type <T> ();
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
void Archive::archive (const char* name, T*& p)
{
  Type const& type = concrete_type <T> ();
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
void Archive::archive (const char* name, std::vector<T>& value)
{
  begin_array_archive (name);

  if (is_reading ())
  {
    while(read_array_has_next (name))
    {
      T tmpVal = detail::GetInitialValue <T> ();
      (*this) ("", tmpVal);
      value.push_back(tmpVal);
    }
  }
  else
  {
    for (auto& v : value)
      (*this) ("", v);
  }
  
  end_array_archive (name);
}

template <class T>
void Archive::archive (const char* name, Range<T>& range)
{
  begin_array_archive (name);

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

  end_array_archive (name);
}

}// end of namespace moose