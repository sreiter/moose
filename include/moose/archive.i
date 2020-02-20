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

namespace moose
{

template <class T>
void Archive::operator () (const char* name, T& value)
{
  if(m_reading)
  {
    begin_read (name);
    read (name, value);
    end_read (name);
  }
  else
  {
    // write (name, value);
  }
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
    read (name, value);
  }
  catch(ArchiveError&)
  {
    value = defVal;
  }

  end_read (name);
}

template <class T>
void Archive::read (const char* name, T& value)
{
///todo: check for POD types and raise an error (->unsupported POD type)
  Serialize (*this, value);
}

template <class T>
void Archive::read (const char* name, std::shared_ptr<T>& sp)
{
  auto const& type = Types::get (get_type_name ());
  if(sp == nullptr)
    sp = type.make_shared <T> ();

  type.serialize (*this, *sp);
}

template <class T>
void Archive::read (const char* name, std::unique_ptr<T>& up)
{
  auto const& type = Types::get (get_type_name ());
  if(up == nullptr)
    up = type.make_unique <T> ();

  type.serialize (*this, *up);
}

template <class T>
void Archive::read (const char* name, T*& p)
{
  auto const& type = Types::get (get_type_name ());
  if(!p)
    p = type.make_raw <T> ();

  type.serialize (*this, *p);
}


template <class T>
void Archive::read (const char* name, std::vector<T>& value)
{
  begin_array_read (name);

  while(array_has_next (name))
  {
    T tmpVal;
    (*this) ("", tmpVal);
    value.push_back(tmpVal);
  }
  
  end_array_read (name);
}

}// end of namespace moose