// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#include <moose/type.h>

namespace moose
{

Type::Type (std::string name,
            std::vector <std::shared_ptr <Type>> baseClasses,
            make_raw_fnc_t makeRawFnc,
            serialize_fnc_t serializeFnc)
  : m_name (std::move (name))
  , m_baseClasses (std::move (baseClasses))
  , m_makeRawFnc (makeRawFnc)
  , m_serializeFnc (serializeFnc)
{
}

auto Type::name () const -> std::string const&
{
  return m_name;
}

bool Type::is_abstract () const
{
  return m_makeRawFnc != nullptr;
}

bool Type::has_base_class (std::string const& name) const
{
  for(auto const& type : m_baseClasses)
  {
    if(type->name () == name)
      return true;
  }

  for(auto const& type : m_baseClasses)
  {
    if (type->has_base_class (name))
      return true;
  }

  return false;
}

}// end of namespace moose