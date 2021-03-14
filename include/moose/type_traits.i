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

#include <array>
#include <string>
#include <vector>
#include <map>

#include <moose/type_traits.h>

namespace moose
{
  template <>
  struct TypeTraits <bool>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <char>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <unsigned char>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <long int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <long long int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <unsigned int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <unsigned long int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <unsigned long long int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <float>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <double>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <std::string>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <class T>
  struct TypeTraits <std::vector <T>>
  {static constexpr EntryType entryType = EntryType::Vector;};

  template <class T, size_t n>
  struct TypeTraits <std::array <T, n>>
  {static constexpr EntryType entryType = EntryType::Range;};

  template <class Key, class Value, class Compare, class Allocator>
  struct TypeTraits <std::map <Key, Value, Compare, Allocator>>
  {static constexpr EntryType entryType = EntryType::Vector;};

  // std::map <Key, Value>::value_type has a const key and is thus not suitable for deserialization
  template <class Key, class Value, class Compare, class Allocator>
  struct VectorTraits <std::map <Key, Value, Compare, Allocator>>
  {
    using ValueType = std::pair <Key, Value>;
  };

  template <class Key, class Value, class Compare, class Allocator>
  void VectorPushBack (std::map <Key, Value, Compare, Allocator>& map, std::pair <Key, Value> const& value)
  {
    map.emplace (value);
  }
}// end of namespace moose
