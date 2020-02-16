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

auto ObjectFactory::entry_map () -> entry_map_t&
{
  return inst().m_entryMap;
}

auto ObjectFactory::typename_map () -> typename_map_t&
{
  return inst().m_typenameMap;
}

bool ObjectFactory::is_base (Entry& e, const std::string& baseName)
{
  for(auto& name : e.baseClasses)
  {
    if(name == baseName)
      return true;

    Entry& e = entry_map()[name];
    if(is_base(e, baseName))
      return true;
  }

  return false;
}

}// end of namespace
