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

#include <stdexcept>
#include <string>
#include <vector>
#include <typeinfo>

namespace moose {

// moose exceptions derive from this class
class Exception : public std::runtime_error
{
public:
  Exception () : std::runtime_error ("")
  {}

  const char* what () const noexcept override
  {
    return m_what.c_str();
  }

  template <class T>
  Exception& operator << (const T& t)
  {
    what ().append (to_string (t));
    return *this;
  }

  Exception& operator << (const char* t)
  {
    what ().append (t);
    return *this;
  }

private:
  auto what () -> std::string&
  {
    if (m_what.empty ())
      m_what.append (typeid (*this).name ()).append (": ");
    return m_what;
  }

  template <class T>
  auto to_string (const T& t) const -> std::string
  {
    return std::to_string (t);
  }

  auto to_string (const std::string& s) const -> std::string const&
  {
    return s;
  }

  auto to_string (const char* s) const -> std::string
  {
    return s;
  }

  template <class T>
  auto to_string (const std::vector <T>& v) -> std::string
  {
    std::string s ("{");
    for(size_t i = 0; i < v.size(); ++i) {
        if (i > 0) {
            s.append (", ");
        }
        s.append (to_string (v[i]));
    }
    s.append ("}");
    return s;
  }

private:
  std::string m_what;
};

class ArchiveError : public Exception {};
class FactoryError : public Exception {};
class TypeError : public Exception {};

}// end of namespace moose
