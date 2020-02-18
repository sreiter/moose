// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

// #include <typeinfo>
#include <type_traits>

#include <moose/object_factory.h>
#include <moose/exceptions.h>
#include <moose/serialize.h>
#include <moose/type.h>

namespace moose
{

template <class T>
Type& ObjectFactory::add (std::string name)
{
  return add <T> (std::move (name), {}, &CallSerialize<T>);
}

template <class T, class TBase1, class... TBaseOthers>
Type& ObjectFactory::add (std::string name)
{
  return add <T, TBase1, TBaseOthers...> (std::move (name), &CallSerialize<T>);
}

template <class T>
Type& ObjectFactory::add_without_serialize (std::string name)
{
  return add <T> (std::move (name), {}, nullptr);
}

template <class T, class TBase1, class... TBaseOthers>
Type& ObjectFactory::add_without_serialize (std::string name)
{
  return add <T, TBase1, TBaseOthers...> (std::move (name), nullptr);
}

template <class T, class TBase1, class... TBaseOthers>
Type& ObjectFactory::add (std::string name, serialize_fnc_t serializeFnc)
{
  types_t baseClasses;
  collect_types <TBase1, TBaseOthers...> (baseClasses);
  return add <T> (std::move (name), std::move (baseClasses), serializeFnc);
}

template <class T>
Type& ObjectFactory::get ()
{
  return *get_shared <T> ();
}

template <class T>
std::shared_ptr <Type> ObjectFactory::get_shared ()
{
  auto iter = type_hash_map ().find (typeid (T).hash_code ());
  if (iter == type_hash_map ().end ())
    throw FactoryError () << "Trying to access unregistered type '" << typeid (T).name () << "'.";
  return iter->second;
}

template <class T>
void* ObjectFactory::CreateFunc ()
{
  return new T;
}

template <class T>
void ObjectFactory::CallSerialize (Archive& ar, void* val)
{
  moose::Serialize (ar, *reinterpret_cast<T*>(val));
}

template <class T>
auto ObjectFactory::add (std::string name,
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
auto ObjectFactory::add (std::string name,
                         types_t baseClasses,
                         serialize_fnc_t serializeFnc)
-> typename std::enable_if <std::is_abstract <T>::value, Type&>::type
{
  auto type = std::make_shared <Type> (std::move (name),
                                       std::move (baseClasses),
                                       nullptr,
                                       serializeFnc)
  auto& ref = *type;
  add <T> (std::move (type));
  return ref;
}

template <class T>
void ObjectFactory::add (std::shared_ptr <Type> type)
{
  if (type_name_map ().count (type->name ()) > 0)
    throw FactoryError () << "Type '" << type->name () << "' has already been registered.";

  type_name_map () [type->name ()] = type;
  type_hash_map () [typeid (T).hash_code()] = std::move (type);
}

template <class T>
void ObjectFactory::collect_types (types_t& typesOut)
{
  typesOut.push_back(get_shared <T> ());
}

template <class HEAD, class TBase2, class... TBaseOthers>
void ObjectFactory::collect_types (types_t& typesOut)
{
  collect_types <HEAD> (typesOut);
  collect_types <TBase2, TBaseOthers...> (typesOut);
}

template <class... T>
void AddType (std::string name)
{
  ObjectFactory::add <T...> (std::move (name));
}

template <class... T>
void AddTypeWithoutSerialize (std::string name)
{
  ObjectFactory::add_without_serialize <T...> (std::move (name));
}

}// end of namespace moose