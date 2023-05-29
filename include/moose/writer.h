// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020-2022 Sebastian Reiter <s.b.reiter@gmail.com>
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

#include <string>
#include <moose/type_traits.h>
#include <moose/version.h>

namespace moose
{
  enum class Hint
  {
    None,
    OneLine,
    ChildrenOneLine
  };
  
  /** \brief Abstract base class for the implementation of writers for specific format.
    An instance of a concrete derived class is passed to an `Archive` to perform serialization.
  */
  class Writer
  {
  public:
    virtual ~Writer ();

    /** \brief Returns false if the specified entry was not found.
      If false was returned, no matching `end_entry` may be called.*/
    virtual bool begin_entry (const char* name, EntryType entryType, Hint hint) = 0;
    virtual void end_entry (const char* name, EntryType entryType) = 0;

    virtual void write_type_name (std::string const& typeName) = 0;
    virtual void write_type_version (Version const& version) = 0;

    virtual void write (const char* name, bool val) = 0;
    virtual void write (const char* name, double val) = 0;
    virtual void write (const char* name, std::string const& val) = 0;

  /** \brief writes a number value (int, float, ...).
    Default implementation redirects to 'write (const char*, double&)'
    \{ */
    virtual void write (const char* name, char val);
    virtual void write (const char* name, unsigned char val);
    virtual void write (const char* name, int val);
    virtual void write (const char* name, long int val);
    virtual void write (const char* name, long long int val);
    virtual void write (const char* name, unsigned int val);
    virtual void write (const char* name, unsigned long int val);
    virtual void write (const char* name, unsigned long long int val);
    virtual void write (const char* name, float val);
  /** \} */

  private:
    template <class T>
    void write_double (const char* name, T val);
  };
}// end of namespace moose
