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

namespace moose
{

template <class T>
void ObjectFactory::add (std::string name)
{
  add <T> (std::move (name), &CallSerialize<T>);
}

template <class T, class TBase1, class... TBaseOthers>
void ObjectFactory::add (std::string name)
{
  Type& type = add <T>(std::move (name), &CallSerialize<T>);
  add_base_class<TBase1, TBaseOthers ...> (type);
}

template <class T>
void ObjectFactory::add_without_serialize (std::string name)
{
  add <T> (std::move (name), nullptr);
}

template <class T, class TBase1, class... TBaseOthers>
void ObjectFactory::add_without_serialize (std::string name)
{
  Type& type = add <T>(std::move (name), nullptr);
  add_base_class<TBase1, TBaseOthers ...> (type);
}

template <class TBase>
TBase* ObjectFactory::create (const std::string& name)
{
  using namespace std;
  type_map_t::iterator i = type_map().find(name);
  if (i == type_map().end())
    throw FactoryError () << "Can't create object for unregistered type '" << name<< "'";

  Type& type = i->second;
  if (!type.createFnc)
    throw FactoryError () << "Can't create instance of abstract type '" << name << "'";

//  Make sure that we may convert the type we intend to create to 'TBase*'
  const std::string& baseName = get_typename<TBase> ();
  if (baseName.empty())
    throw FactoryError () << "Can't convert type '" << name << "' to unregistered type '"
                          << typeid(TBase).name() << "'";


  if ((name != baseName) && !is_base(type, baseName))
    throw FactoryError () << "Can't convert type '" << name << "' to type '" << baseName << "' ";

//todo: this may cause problem in cases of multiple inheritance.
//    Think abount storing cast-methods between types and registered base classes.
  return reinterpret_cast<TBase*> (type.createFnc());
}

template <class TBase>
void ObjectFactory::call_serialize (const std::string& name, Archive& ar, TBase* b)
{
  using namespace std;
  type_map_t::iterator i = type_map().find(name);
  if (i == type_map().end())
    throw FactoryError () << "Can't create object for unregistered type '" << name << "'";

  Type& type = i->second;
  if (type.serializeFnc != nullptr)
    type.serializeFnc (ar, b);
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
auto ObjectFactory::get_typename () -> std::string&
{
  return typename_map()[typeid(T).hash_code()];
}

template <class T>
auto ObjectFactory::add (std::string name, serialize_fnc_t serializeFnc)
-> typename std::enable_if <!std::is_abstract <T>::value, Type&>::type
{
  return add <T> (std::move (name), serializeFnc, &CreateFunc <T>);
}

template <class T>
auto ObjectFactory::add (std::string name, serialize_fnc_t serializeFnc)
-> typename std::enable_if <std::is_abstract <T>::value, Type&>::type
{
  return add <T> (std::move (name), serializeFnc, nullptr);
}

template <class T>
auto ObjectFactory::add (std::string name, serialize_fnc_t serializeFnc, create_fnc_t createFnc)
-> Type&
{
  Type& type = type_map()[name];
  typename_map()[typeid(T).hash_code()] = name;
  type.name = std::move (name);
  type.createFnc = createFnc;
  type.serializeFnc = serializeFnc;
  return type;
}

template <class T>
void ObjectFactory::add_base_class (Type& type)
{
  using namespace std;
  const std::string& baseName = get_typename<T>();
  if (baseName.empty())
    throw FactoryError () << "Can't associate unregistered base class of type '"
                          << typeid(T).name() << "' with newly registered type '"
                          << type.name << "'";

  type.baseClasses.push_back(baseName);
}

template <class HEAD, class TBase2, class... TBaseOthers>
void ObjectFactory::add_base_class (Type& type)
{
  add_base_class <HEAD> (type);
  add_base_class <TBase2, TBaseOthers...> (type);
}

template <class... T>
void AddType (std::string name)
{
  ObjectFactory::add <T...>(std::move (name));
}

template <class... T>
void AddTypeWithoutSerialize (std::string name)
{
  ObjectFactory::add_without_serialize <T...>(std::move (name));
}

}// end of namespace moose