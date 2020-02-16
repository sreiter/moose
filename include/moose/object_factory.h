// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

#include <map>
#include <string>
#include <vector>

namespace moose
{

class Archive;

class ObjectFactory
{
public:
  using create_fnc_t    = void* (*)();
  using serialize_fnc_t = void (*)(Archive&, void*);

  struct Type
  {
    create_fnc_t              createFnc;
    serialize_fnc_t           serializeFnc;
    std::string               name;
    std::vector <std::string> baseClasses;
  };

  template <class T>
  static void add (std::string name);

  template <class T, class TBase1, class... TBaseOthers>
  static void add (std::string name);

  template <class T>
  static void add_without_serialize (std::string name);

  template <class T, class TBase1, class... TBaseOthers>
  static void add_without_serialize (std::string name);

  template <class TBase>
  static TBase* create (const std::string& name);

  template <class TBase>
  static void call_serialize (const std::string& name, Archive& ar, TBase* b);

private:
  using type_map_t     = std::map <std::string, Type>;
  using typename_map_t = std::map <std::size_t, std::string>;

private:
  ObjectFactory () = default;

  template <class T>
  static void* CreateFunc ();

  template <class T>
  static void CallSerialize (Archive& ar, void* val);

  static ObjectFactory& inst ();

  static type_map_t& type_map ();

  static typename_map_t& typename_map ();

  template <class T>
  static std::string& get_typename ();

  static bool is_base (Type& type, const std::string& baseName);

  template <class T>
  static typename std::enable_if <!std::is_abstract <T>::value, Type&>::type
  add (std::string name, serialize_fnc_t serializeFnc);

  template <class T>
  static typename std::enable_if <std::is_abstract <T>::value, Type&>::type
  add (std::string name, serialize_fnc_t serializeFnc);

  template <class T>
  static Type&
  add (std::string name, serialize_fnc_t serializeFnc, create_fnc_t createFnc);

  template <class T>
  static void add_base_class (Type& type);

  template <class HEAD, class TBase2, class... TBaseOthers>
  static void add_base_class (Type& type);

private:
  type_map_t     m_typeMap;
  typename_map_t m_typenameMap;
};

template <class... T>
void AddType (std::string name);

template <class... T>
void AddTypeWithoutSerialize (std::string name);

}// end of namespace

#include <moose/object_factory_impl.h>