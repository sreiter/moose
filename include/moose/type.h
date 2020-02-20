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

#include <memory>
#include <string>
#include <vector>

namespace moose
{

class Archive;

class Type
{
public:
  using make_raw_fnc_t  = void* (*)();
  using serialize_fnc_t = void (*)(Archive&, void*);

  Type (std::string name,
        std::vector <std::shared_ptr <Type>> baseClasses,
        make_raw_fnc_t makeRawFnc,
        serialize_fnc_t serializeFnc);

  auto name () const -> std::string const&;

  /** If the method returns `true` the type is abstract, i.e.,
    no instances can be created through `make_raw/shared/unique`.
    Returns `false` if the type is not abstract, i.e., `make_raw/shared/unique`
    may be called to create instances of the type.*/
  bool is_abstract () const;

  template <class Base>
  Base* make_raw () const;

  template <class Base>
  std::shared_ptr <Base> make_shared () const;

  template <class Base>
  std::unique_ptr <Base> make_unique () const;

  template <class Base>
  void serialize (Archive& ar, Base& b) const;

  template <class Base>
  bool has_base_class () const;

  bool has_base_class (std::string const& name) const;

private:
  template <class Base>
  void throw_on_bad_base_class (const char* what) const;

private:
  std::string         m_name;
  std::vector <std::shared_ptr <Type>> m_baseClasses;
  make_raw_fnc_t      m_makeRawFnc;
  serialize_fnc_t     m_serializeFnc;
};

}// end of namespace

#include <moose/type.i>
