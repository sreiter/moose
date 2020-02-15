// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

#include <moose/serialize.h>
#include <moose/detail/object_factory.h>

namespace moose
{

template <class... T>
void RegisterType (std::string name)
{
  detail::ObjectFactory::register_type <T...>(std::move (name));
}

template <class... T>
void RegisterEmptyType (std::string name)
{
  detail::ObjectFactory::register_empty_type <T...>(std::move (name));
}

}// end of namespace
