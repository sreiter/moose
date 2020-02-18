// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#include <typeinfo>
#include <type_traits>

#include <moose/types.h>
#include <moose/exceptions.h>
#include <moose/serialize.h>

namespace moose
{

Type& Types::get (std::string const& name)
{
  auto iter = type_name_map ().find (name);
  if (iter == type_name_map ().end ())
    throw FactoryError () << "Trying to access unregistered type '" << name << "'.";
  return *iter->second;
}

auto Types::inst () -> Types&
{
  static Types of; return of;
}

auto Types::type_name_map () -> type_name_map_t&
{
  return inst().m_typeNameMap;
}

auto Types::type_hash_map () -> type_hash_map_t&
{
  return inst().m_typeHashMap;
}

}// end of namespace