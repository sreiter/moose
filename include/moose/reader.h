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

#include <moose/content_type.h>
#include <moose/export.h>
#include <moose/hint.h>
#include <moose/version.h>

#include <string>

namespace moose
{
  /** \brief Abstract base class for the implementation of readers for specific formats.
    An instance of a concrete derived class is passed to an `Archive` to perform deserialization.
  */
  class Reader
  {
  public:
    MOOSE_EXPORT virtual ~Reader ();

    /** \brief Returns false if the specified entry was not found.
      If false was returned, no matching `end_entry` may be called.*/
    MOOSE_EXPORT virtual bool begin_entry (const char* name, ContentType type) = 0;
    MOOSE_EXPORT virtual void end_entry (const char* name, ContentType type) = 0;

    /** Called between `begin_entry` and `end_entry`.
      Returns `true` if further values are available in the currently read array.
      If the current entry is not an array, the method should return false.*/
    MOOSE_EXPORT virtual bool array_has_next (const char* name) const = 0;

    /** Called between `begin_entry` and `end_entry`.*/
    MOOSE_EXPORT virtual std::string type_name () const = 0;

    /** Called between `begin_entry` and `end_entry`.*/
    MOOSE_EXPORT virtual auto type_version () const -> Version = 0;

    MOOSE_EXPORT virtual void read (const char* name, bool& val) const = 0;
    MOOSE_EXPORT virtual void read (const char* name, double& val) const = 0;
    MOOSE_EXPORT virtual void read (const char* name, std::string& val) const = 0;

  /** \brief reads a number value (int, float, ...).
    Default implementation redirects to 'read (const char*, double&)'
    \{ */
    MOOSE_EXPORT virtual void read (const char* name, char& val) const;
    MOOSE_EXPORT virtual void read (const char* name, unsigned char& val) const;
    MOOSE_EXPORT virtual void read (const char* name, int& val) const;
    MOOSE_EXPORT virtual void read (const char* name, long int& val) const;
    MOOSE_EXPORT virtual void read (const char* name, long long int& val) const;
    MOOSE_EXPORT virtual void read (const char* name, unsigned int& val) const;
    MOOSE_EXPORT virtual void read (const char* name, unsigned long int& val) const;
    MOOSE_EXPORT virtual void read (const char* name, unsigned long long int& val) const;
    MOOSE_EXPORT virtual void read (const char* name, float& val) const;
  /** \} */

  private:
    template <class T>
    void read_double (const char* name, T& val) const;
  };
}// end of namespace moose
