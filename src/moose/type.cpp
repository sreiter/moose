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

#include <moose/type.h>
#include <moose/types.h>

namespace moose
{

  Type::Type (
      std::string name,
      std::type_index typeIndex,
      std::vector<std::type_index> baseClassTypeIndices,
      make_raw_fnc_t makeRawFnc,
      serialize_fnc_t serializeFnc)
    : m_name (std::move (name))
    , m_typeIndex (typeIndex)
    , m_baseClassTypeIndices (std::move (baseClassTypeIndices))
    , m_makeRawFnc (makeRawFnc)
    , m_serializeFnc (serializeFnc)
  {}

  auto Type::name () const -> std::string const&
  {
    return m_name;
}

bool Type::is_abstract () const
{
  return m_makeRawFnc != nullptr;
}

bool Type::has_base_class (std::type_index const& baseClassIndex) const
{
  for(auto const& typeIndex : m_baseClassTypeIndices)
  {
    if(typeIndex == baseClassIndex)
      return true;
  }

  for(auto const& typeIndex : m_baseClassTypeIndices)
  {
    if (types ().get (typeIndex).has_base_class (baseClassIndex))
      return true;
  }

  return false;
}

}// end of namespace moose