// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

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