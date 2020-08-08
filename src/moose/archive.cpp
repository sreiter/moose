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

#include <moose/archive.h>

namespace moose{

Archive::Archive () = default;

Archive::~Archive () = default;

bool Archive::is_reading () const
{
  return mode () == Mode::Read;
}

bool Archive::is_writing () const
{
  return mode () == Mode::Write;
}

void Archive::begin_archive (const char* name)
{}

void Archive::end_archive (const char* name)
{}

void Archive::begin_array_archive (const char* name)
{}

void Archive::end_array_archive (const char* name)
{}

template <class T>
void Archive::archive_double(const char* name, T& val)
{
  if (is_reading ())
  {
    double d;
    archive (name, d);
    val = static_cast <T> (d);
  }
  else
  {
    double d = static_cast <double> (val);
    archive (name, d);
  }
}

void Archive::archive (const char* name, bool& val)
{archive_double (name, val);}

void Archive::archive (const char* name, char& val)
{archive_double (name, val);}

void Archive::archive (const char* name, unsigned char& val)
{archive_double (name, val);}

void Archive::archive (const char* name, int& val)
{archive_double (name, val);}

void Archive::archive (const char* name, long int& val)
{archive_double (name, val);}

void Archive::archive (const char* name, long long int& val)
{archive_double (name, val);}

void Archive::archive (const char* name, unsigned int& val)
{archive_double (name, val);}

void Archive::archive (const char* name, unsigned long int& val)
{archive_double (name, val);}

void Archive::archive (const char* name, unsigned long long int& val)
{archive_double (name, val);}

void Archive::archive (const char* name, float& val)
{archive_double (name, val);}

}// end of namespace moose
