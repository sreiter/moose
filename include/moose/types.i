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

#include <type_traits>

#include <moose/exceptions.h>
#include <moose/serialize.h>
#include <moose/type.h>
#include <moose/types.h>

namespace moose
{

template <class T>
Type& Types::add (std::string name)
{
  return add <T> (std::move (name), {}, &CallSerialize<T>);
}

template <class T, class TBase1, class... TBaseOthers>
Type& Types::add (std::string name)
{
  return add <T, TBase1, TBaseOthers...> (std::move (name), &CallSerialize<T>);
}

template <class T>
Type& Types::add_without_serialize (std::string name)
{
  return add <T> (std::move (name), {}, nullptr);
}

template <class T, class TBase1, class... TBaseOthers>
Type& Types::add_without_serialize (std::string name)
{
  return add <T, TBase1, TBaseOthers...> (std::move (name), nullptr);
}

template <class T, class TBase1, class... TBaseOthers>
Type& Types::add (std::string name, serialize_fnc_t serializeFnc)
{
  types_t baseClasses;
  collect_types <TBase1, TBaseOthers...> (baseClasses);
  return add <T> (std::move (name), std::move (baseClasses), serializeFnc);
}

template <class T>
Type& Types::get ()
{
  return *get_shared <T> ();
}

template <class T>
std::shared_ptr <Type> Types::get_shared ()
{
  auto iter = type_hash_map ().find (typeid (T).hash_code ());
  if (iter == type_hash_map ().end ())
    throw FactoryError () << "Trying to access unregistered type '" << typeid (T).name () << "'.";
  return iter->second;
}

template <class T>
void* Types::CreateFunc ()
{
  return new T;
}

template <class T>
void Types::CallSerialize (Archive& ar, void* val)
{
  moose::Serialize (ar, *reinterpret_cast<T*>(val));
}

template <class T>
auto Types::add (std::string name,
                 types_t baseClasses,
                 serialize_fnc_t serializeFnc)
-> typename std::enable_if <!std::is_abstract <T>::value, Type&>::type
{
  auto type = std::make_shared <Type> (std::move (name),
                                       std::move (baseClasses),
                                       &CreateFunc <T>,
                                       serializeFnc);
  auto& ref = *type;
  add <T> (std::move (type));
  return ref;
}

template <class T>
auto Types::add (std::string name,
                 types_t baseClasses,
                 serialize_fnc_t serializeFnc)
-> typename std::enable_if <std::is_abstract <T>::value, Type&>::type
{
  auto type = std::make_shared <Type> (std::move (name),
                                       std::move (baseClasses),
                                       nullptr,
                                       serializeFnc);
  auto& ref = *type;
  add <T> (std::move (type));
  return ref;
}

template <class T>
void Types::add (std::shared_ptr <Type> type)
{
  if (type_name_map ().count (type->name ()) > 0)
    throw FactoryError () << "Type '" << type->name () << "' has already been registered.";

  type_name_map () [type->name ()] = type;
  type_hash_map () [typeid (T).hash_code()] = std::move (type);
}

template <class T>
void Types::collect_types (types_t& typesOut)
{
  typesOut.push_back(get_shared <T> ());
}

template <class HEAD, class TBase2, class... TBaseOthers>
void Types::collect_types (types_t& typesOut)
{
  collect_types <HEAD> (typesOut);
  collect_types <TBase2, TBaseOthers...> (typesOut);
}

}// end of namespace moose