// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#include <typeinfo>
#include <type_traits>

#include <moose/object_factory.h>
#include <moose/exceptions.h>
#include <moose/serialize.h>

namespace moose
{

auto ObjectFactory::inst () -> ObjectFactory&
{
  static ObjectFactory of; return of;
}

auto ObjectFactory::type_map () -> type_map_t&
{
  return inst().m_typeMap;
}

auto ObjectFactory::typename_map () -> typename_map_t&
{
  return inst().m_typenameMap;
}

bool ObjectFactory::is_base (Type& type, const std::string& baseName)
{
  for(auto& name : type.baseClasses)
  {
    if(name == baseName)
      return true;

    Type& type = type_map()[name];
    if(is_base(type, baseName))
      return true;
  }

  return false;
}

}// end of namespace
