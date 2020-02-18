// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

#include <moose/exceptions.h>
#include <moose/object_factory.h>
#include <moose/type.h>

namespace moose
{

template <class Base>
Base* Type::make_raw () const
{
  if (m_makeRawFnc == nullptr)
    throw TypeError () << "Cannot create instance of abstract type " << m_name;

  throw_on_bad_base_class <Base> ("while executing 'Type::make_raw'");

  return reinterpret_cast <Base*> (m_makeRawFnc ());
}

template <class Base>
std::shared_ptr <Base> Type::make_shared () const
{
  return std::shared_ptr <Base> (make_raw <Base> ());
}

template <class Base>
std::unique_ptr <Base> Type::make_unique () const
{
  return std::unique_ptr <Base> (make_raw ());
}

template <class Base>
void Type::serialize (Archive& ar, Base& b) const
{
  if (m_serializeFnc == nullptr)
    throw TypeError () << "Cannot call serialize on '" << m_name
                       << "'. The type was created without serialization capabilities.";

  throw_on_bad_base_class <Base> ("while executing 'Type::serialize'");

  return m_serializeFnc (ar, &b);
}

template <class Base>
bool Type::has_base_class () const
{
  return has_base_class (ObjectFactory::get <Base> ().name ());
}

template <class Base>
void Type::throw_on_bad_base_class (const char* what) const
{
  if (!has_base_class <Base> ())
    throw TypeError () << "Cannot cast instance of type '" << m_name << "' to type '"
                       << ObjectFactory::get <Base> ().name () << "' " << what;
}

}
