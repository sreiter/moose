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

#include <moose/types.h>
#include <moose/exceptions.h>
#include <moose/serialize.h>

namespace moose
{

Type& Types::get (std::string const& name)
{
  auto iter = m_typeNameMap.find (name);
  if (iter == m_typeNameMap.end ())
    throw FactoryError () << "Trying to access unregistered type '" << name << "'.";
  return *iter->second;
}

Type& Types::get (std::type_index const& typeIndex)
{
  auto iter = m_typeIndexMap.find (typeIndex);
  if (iter == m_typeIndexMap.end ())
    throw FactoryError () << "Trying to access unregistered type '" << typeIndex.name () << "'.";
  return *iter->second;
}

Type* Types::get_if (std::string const& name)
{
  auto iter = m_typeNameMap.find (name);
  if (iter == m_typeNameMap.end ())
    return nullptr;
  return iter->second.get ();
}

Type* Types::get_if (std::type_index const& typeIndex)
{
  auto iter = m_typeIndexMap.find (typeIndex);
  if (iter == m_typeIndexMap.end ())
    return nullptr;
  return iter->second.get ();
}

auto types () -> Types&
{
  static Types out;
  return out;
}

}// end of namespace
