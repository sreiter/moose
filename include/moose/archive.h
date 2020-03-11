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

#include <map>
#include <memory>
#include <string>
#include <moose/range.h>
#include <moose/types.h>

namespace moose
{

template <class T>
struct RangeSerialization
{
  static constexpr bool enabled = false;
};

class Archive
{
public:
  enum class Mode
  {
    Read,
    Write
  };

public:
  Archive (Mode mode);

  virtual ~Archive ();

  template <class T>
  void operator () (const char* name, T& value);

  /// Read with default value
  template <class T>
  void operator () (const char* name, T& value, const T& defVal);

  Mode mode () const;

  bool is_reading () const;
  bool is_writing () const;
  
protected:
  virtual void begin_read (const char* name);
  virtual void end_read (const char* name);

  virtual void begin_array_read (const char* name);
  virtual bool array_has_next (const char* name) = 0;
  virtual void end_array_read (const char* name);

  virtual std::string get_type_name () = 0;

/// reads a number value (int, float, ...).
/** Default implementation redirects to 'read(const char*, double&)'
 * \{ */
  virtual void read (const char* name, bool& val);
  virtual void read (const char* name, char& val);
  virtual void read (const char* name, unsigned char& val);
  virtual void read (const char* name, int& val);
  virtual void read (const char* name, long int& val);
  virtual void read (const char* name, long long int& val);
  virtual void read (const char* name, unsigned int& val);
  virtual void read (const char* name, unsigned long int& val);
  virtual void read (const char* name, unsigned long long int& val);
  virtual void read (const char* name, float& val);
  virtual void read (const char* name, double& val) = 0;
/** \} */

  virtual void read (const char* name, std::string& val) = 0;

protected:
/** If a concrete type is defined by the current entry in the archive,
  the corresponding Type object is returned. If not, the Type object
  corresponding to the given template argument is returned.
*/
  template <class T>
  Type const& concrete_type ();

  template <class T>
  void read (const char* name, T& value);

  template <class T>
  void read (const char* name, std::shared_ptr<T>& sp);

  template <class T>
  void read (const char* name, std::unique_ptr<T>& up);

  template <class T>
  void read (const char* name, T*& p);

  template <class T>
  void read (const char* name, std::vector<T>& value);

  template <class T>
  void read (const char* name, Range<T>& value);

private:
  template <class T>
  void read_double(const char* name, T& val);

  Mode m_mode;
};

}// end of namespace moose

#include <moose/archive.i>
