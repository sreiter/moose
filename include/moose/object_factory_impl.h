// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

#include <typeinfo>
#include <type_traits>

#include <moose/object_factory.h>
#include <moose/exceptions.h>
#include <moose/serialize.h>

namespace moose
{

template <class T>
void ObjectFactory::register_type (std::string name)
{
  add_entry<T> (std::move (name), &CallSerialize<T>);
}

template <class T, class TBase1, class... TBaseOthers>
void ObjectFactory::register_type (std::string name)
{
  Entry& e = add_entry<T>(std::move (name), &CallSerialize<T>);
  add_base_class<TBase1, TBaseOthers ...> (e);
}

template <class T>
void ObjectFactory::register_empty_type (std::string name)
{
  add_entry<T> (std::move (name), nullptr);
}

template <class T, class TBase1, class... TBaseOthers>
void ObjectFactory::register_empty_type (std::string name)
{
  Entry& e = add_entry<T>(std::move (name), nullptr);
  add_base_class<TBase1, TBaseOthers ...> (e);
}

template <class TBase>
TBase* ObjectFactory::create (const std::string& name)
{
  using namespace std;
  entry_map_t::iterator i = entry_map().find(name);
  if (i == entry_map().end())
    throw FactoryError () << "Can't create object for unregistered type '" << name<< "'";

  Entry& e = i->second;
  if (!e.createFnc)
    throw FactoryError () << "Can't create instance of abstract type '" << name << "'";

//  Make sure that we may convert the type we intend to create to 'TBase*'
  const std::string& baseName = get_typename<TBase> ();
  if (baseName.empty())
    throw FactoryError () << "Can't convert type '" << name << "' to unregistered type '"
                          << typeid(TBase).name() << "'";


  if ((name != baseName) && !is_base(e, baseName))
    throw FactoryError () << "Can't convert type '" << name << "' to type '" << baseName << "' ";

//todo: this may cause problem in cases of multiple inheritance.
//    Think abount storing cast-methods between types and registered base classes.
  return reinterpret_cast<TBase*> (e.createFnc());
}

template <class TBase>
void ObjectFactory::call_serialize (const std::string& name, Archive& ar, TBase* b)
{
  using namespace std;
  entry_map_t::iterator i = entry_map().find(name);
  if (i == entry_map().end())
    throw FactoryError () << "Can't create object for unregistered type '" << name << "'";

  Entry& e = i->second;
  if (e.serializeFnc != nullptr)
    e.serializeFnc (ar, b);
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
auto ObjectFactory::add_entry (std::string name, serialize_fnc_t serializeFnc)
-> typename std::enable_if <!std::is_abstract <T>::value, Entry&>::type
{
  return add_entry <T> (std::move (name), serializeFnc, &CreateFunc <T>);
}

template <class T>
auto ObjectFactory::add_entry (std::string name, serialize_fnc_t serializeFnc)
-> typename std::enable_if <std::is_abstract <T>::value, Entry&>::type
{
  return add_entry <T> (std::move (name), serializeFnc, nullptr);
}

template <class T>
auto ObjectFactory::add_entry (std::string name, serialize_fnc_t serializeFnc, create_fnc_t createFnc)
-> Entry&
{
  Entry& e = entry_map()[name];
  typename_map()[typeid(T).hash_code()] = name;
  e.name = std::move (name);
  e.createFnc = createFnc;
  e.serializeFnc = serializeFnc;
  return e;
}

template <class T>
void ObjectFactory::add_base_class (Entry& e)
{
  using namespace std;
  const std::string& baseName = get_typename<T>();
  if (baseName.empty())
    throw FactoryError () << "Can't associate unregistered base class of type '"
                          << typeid(T).name() << "' with newly registered type '" << e.name << "'";

  e.baseClasses.push_back(baseName);
}

template <class HEAD, class TBase2, class... TBaseOthers>
void ObjectFactory::add_base_class (Entry& e)
{
  add_base_class <HEAD> (e);
  add_base_class <TBase2, TBaseOthers...> (e);
}

template <class... T>
void RegisterType (std::string name)
{
  ObjectFactory::register_type <T...>(std::move (name));
}

template <class... T>
void RegisterEmptyType (std::string name)
{
  ObjectFactory::register_empty_type <T...>(std::move (name));
}

}// end of namespace moose