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

#include <moose/archive.h>

#include <moose/stl/array.h>
#include <moose/stl/optional.h>
#include <moose/stl/variant.h>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace moose
{
  template <class FIRST, class SECOND>
  void Serialize (
    Archive& archive,
    std::pair <FIRST, SECOND>& value)
  {
    archive ("key", value.first);
    archive ("value", value.second);
  }

  template <class T, class Allocator>
  struct TypeTraits <std::vector <T, Allocator>>
  {
    using Type = std::vector <T, Allocator>;
    using ValueType = typename Type::value_type;

    static constexpr EntryType entryType = EntryType::Vector;

    static constexpr bool wantsToUnpack = true;
    
    static auto toRange (Type& vector) -> Range <decltype (vector.begin ())>
    {
      return {vector.begin (), vector.end ()};
    }

    static void pushBack (Type& vector, ValueType const& value)
    { vector.push_back (value); }

    static void clear (Type& vector)
    { vector.clear (); }
  };

  template <class Key, class Value, class Compare, class Allocator>
  struct TypeTraits <std::map <Key, Value, Compare, Allocator>>
  {
    using Type = std::map <Key, Value, Compare, Allocator>;
    // std::map <Key, Value>::value_type has a const key and is thus not suitable for deserialization
    using ValueType = std::pair <Key, Value>;

    static constexpr EntryType entryType = EntryType::Vector;

    static auto toRange (Type& map) -> Range <decltype (map.begin ())>
    {
      return {map.begin (), map.end ()};
    }

    static void pushBack (Type& map, ValueType const& value)
    { map.insert (value); }

    static void clear (Type& map)
    { map.clear (); }
  };

  template <class Key, class Compare, class Allocator>
  struct TypeTraits <std::set <Key, Compare, Allocator>>
  {
    using Type = std::set <Key, Compare, Allocator>;
    using ValueType = typename Type::value_type;

    static constexpr EntryType entryType = EntryType::Vector;

    static auto toRange (Type& set) -> Range <decltype (set.begin ())>
    {
      return {set.begin (), set.end ()};
    }

    static void pushBack (Type& set, ValueType const& value)
    { set.insert (value); }

    static void clear (Type& set)
    { set.clear (); }
  };
}// end of namespace
