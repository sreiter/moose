// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#include <moose/archive.h>

namespace moose{

Archive::Archive (bool reading)
  : m_reading (reading)
{}

Archive::~Archive ()
{}

void Archive::begin_read (const char* name)
{}

void Archive::end_read (const char* name)
{}

void Archive::begin_array_read (const char* name)
{}

void Archive::end_array_read (const char* name)
{}

template <class T>
void Archive::read_double(const char* name, T& val)
{
  double d;
  read (name, d);
  val = static_cast <T> (d);
}

void Archive::read (const char* name, char& val)
{read_double (name, val);}

void Archive::read (const char* name, unsigned char& val)
{read_double (name, val);}

void Archive::read (const char* name, int& val)
{read_double (name, val);}

void Archive::read (const char* name, long int& val)
{read_double (name, val);}

void Archive::read (const char* name, long long int& val)
{read_double (name, val);}

void Archive::read (const char* name, unsigned int& val)
{read_double (name, val);}

void Archive::read (const char* name, unsigned long int& val)
{read_double (name, val);}

void Archive::read (const char* name, unsigned long long int& val)
{read_double (name, val);}

void Archive::read (const char* name, float& val)
{read_double (name, val);}


}// end of namespace moose
