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
  template <class T>
  static void register_type (std::string name);

  template <class T, class TBase1, class... TBaseOthers>
  static void register_type (std::string name);

  template <class T>
  static void register_empty_type (std::string name);

  template <class T, class TBase1, class... TBaseOthers>
  static void register_empty_type (std::string name);

  template <class TBase>
  static TBase* create (const std::string& name);

  template <class TBase>
  static void call_serialize (const std::string& name, Archive& ar, TBase* b);

private:
  using create_fnc_t    = void* (*)();
  using serialize_fnc_t = void (*)(Archive&, void*);

  struct Entry
  {
    create_fnc_t              createFnc;
    serialize_fnc_t           serializeFnc;
    std::string               name;
    std::vector <std::string> baseClasses;
  };

  using entry_map_t    = std::map<std::string, Entry>;
  using typename_map_t = std::map<std::size_t, std::string>;

private:
  ObjectFactory () = default;

  template <class T>
  static void* CreateFunc ();

  template <class T>
  static void CallSerialize (Archive& ar, void* val);

  static ObjectFactory& inst ();

  static entry_map_t& entry_map ();

  static typename_map_t& typename_map ();

  template <class T>
  static std::string& get_typename ();

  static bool is_base (Entry& e, const std::string& baseName);

  template <class T>
  static typename std::enable_if <!std::is_abstract <T>::value, Entry&>::type
  add_entry (std::string name, serialize_fnc_t serializeFnc);

  template <class T>
  static typename std::enable_if <std::is_abstract <T>::value, Entry&>::type
  add_entry (std::string name, serialize_fnc_t serializeFnc);

  template <class T>
  static Entry&
  add_entry (std::string name, serialize_fnc_t serializeFnc, create_fnc_t createFnc);

  template <class T>
  static void add_base_class (Entry& e);

  template <class HEAD, class TBase2, class... TBaseOthers>
  static void add_base_class (Entry& e);

private:
  entry_map_t   m_entryMap;
  typename_map_t  m_typenameMap;
};

template <class... T>
void RegisterType (std::string name);

template <class... T>
void RegisterEmptyType (std::string name);

}// end of namespace

#include <moose/object_factory_impl.h>