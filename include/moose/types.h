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
#include <typeindex>
#include <vector>

#include <moose/export.h>

namespace moose
{

class Archive;
class Type;

class Types
{
public:
  Types () = default;

  template <class T>
  Type& add (std::string name);

  template <class T, class TBase1, class... TBaseOthers>
  Type& add (std::string name);

  template <class T>
  Type& add_without_serialize (std::string name);

  template <class T, class TBase1, class... TBaseOthers>
  Type& add_without_serialize (std::string name);

  template <class T>
  Type& get ();

  MOOSE_EXPORT Type& get (std::string const& name);

  MOOSE_EXPORT Type& get (std::type_index const& typeIndex);

  /// Returns a pointer to the queried type or `nullptr` if no type was registered for the given `name`.
  MOOSE_EXPORT Type* get_if (std::string const& name);

  /// Returns a pointer to the queried type or `nullptr` if no type was registered for the given `typeIndex`.
  MOOSE_EXPORT Type* get_if (std::type_index const& typeIndex);

  template <class T>
  std::shared_ptr <Type> get_shared ();

  template <class T>
  Type& get_polymorphic (T& derived);

private:
  using make_raw_fnc_t  = void* (*)();
  using serialize_fnc_t = void (*)(Archive&, void*);

  using type_name_map_t = std::map <std::string, std::shared_ptr <Type>>;
  using type_index_map_t = std::map <std::type_index, std::shared_ptr <Type>>;

  using type_indices_t = std::vector <std::type_index>;

private:
  template <class T>
  static void* CreateFunc ();

  template <class T>
  static void CallSerialize (Archive& ar, void* val);

  bool is_base (Type& type, const std::string& baseName);

  template <class T, class TBase1, class... TBaseOthers>
  Type&
  add (std::string name, serialize_fnc_t serializeFnc);

  template <class T>
  typename std::enable_if <std::is_default_constructible <T>::value, Type&>::type
  add (std::string name,
       type_indices_t baseClasses,
       serialize_fnc_t serializeFnc);

  template <class T>
  typename std::enable_if <!std::is_default_constructible <T>::value, Type&>::type
  add (std::string name,
       type_indices_t baseClasses,
       serialize_fnc_t serializeFnc);

  template <class T>
  void
  add (std::shared_ptr <Type> type);

  template <class T>
  void collect_types_indices (type_indices_t& typesOut);

  template <class HEAD, class TBase2, class... TBaseOthers>
  void collect_types_indices (type_indices_t& typesOut);

private:
  type_name_map_t m_typeNameMap;
  type_index_map_t m_typeIndexMap;
};

/// Returns the default `Types` instance.
MOOSE_EXPORT auto types () -> Types&;

}// end of namespace

#include <moose/types.i>
