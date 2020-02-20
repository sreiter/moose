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

#include <moose/exceptions.h>
#include <moose/type.h>
#include <moose/types.h>

namespace moose
{

template <class Base>
Base* Type::make_raw () const
{
  if (m_makeRawFnc == nullptr)
    throw TypeError () << "Cannot create instance of abstract type " << m_name;

  throw_on_bad_base_class <Base> ("while executing 'Type::make_raw'");

  return reinterpret_cast <Base*> (m_makeRawFnc ());
}

template <class Base>
std::shared_ptr <Base> Type::make_shared () const
{
  return std::shared_ptr <Base> (make_raw <Base> ());
}

template <class Base>
std::unique_ptr <Base> Type::make_unique () const
{
  return std::unique_ptr <Base> (make_raw <Base> ());
}

template <class Base>
void Type::serialize (Archive& ar, Base& b) const
{
  if (m_serializeFnc == nullptr)
    throw TypeError () << "Cannot call serialize on '" << m_name
                       << "'. The type was created without serialization capabilities.";

  throw_on_bad_base_class <Base> ("while executing 'Type::serialize'");

  return m_serializeFnc (ar, &b);
}

template <class Base>
bool Type::has_base_class () const
{
  return has_base_class (Types::get <Base> ().name ());
}

template <class Base>
void Type::throw_on_bad_base_class (const char* what) const
{
  if (!has_base_class <Base> ())
    throw TypeError () << "Cannot cast instance of type '" << m_name << "' to type '"
                       << Types::get <Base> ().name () << "' " << what;
}

}
