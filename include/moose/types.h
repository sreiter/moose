// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace moose
{

class Archive;
class Type;

class Types
{
public:
  template <class T>
  static Type& add (std::string name);

  template <class T, class TBase1, class... TBaseOthers>
  static Type& add (std::string name);

  template <class T>
  static Type& add_without_serialize (std::string name);

  template <class T, class TBase1, class... TBaseOthers>
  static Type& add_without_serialize (std::string name);

  template <class T>
  static Type& get ();

  static Type& get (std::string const& name);

  template <class T>
  static std::shared_ptr <Type> get_shared ();

private:
  using make_raw_fnc_t  = void* (*)();
  using serialize_fnc_t = void (*)(Archive&, void*);

  using type_name_map_t = std::map <std::string, std::shared_ptr <Type>>;
  using type_hash_map_t = std::map <std::size_t, std::shared_ptr <Type>>;

  using types_t = std::vector <std::shared_ptr <Type>>;

private:
  Types () = default;

  template <class T>
  static void* CreateFunc ();

  template <class T>
  static void CallSerialize (Archive& ar, void* val);

  static Types& inst ();

  static type_name_map_t& type_name_map ();

  static type_hash_map_t& type_hash_map ();

  static bool is_base (Type& type, const std::string& baseName);

  template <class T, class TBase1, class... TBaseOthers>
  static Type&
  add (std::string name, serialize_fnc_t serializeFnc);

  template <class T>
  static typename std::enable_if <!std::is_abstract <T>::value, Type&>::type
  add (std::string name,
       types_t baseClasses,
       serialize_fnc_t serializeFnc);

  template <class T>
  static typename std::enable_if <std::is_abstract <T>::value, Type&>::type
  add (std::string name,
       types_t baseClasses,
       serialize_fnc_t serializeFnc);

  template <class T>
  static void
  add (std::shared_ptr <Type> type);

  template <class T>
  static void collect_types (types_t& typesOut);

  template <class HEAD, class TBase2, class... TBaseOthers>
  static void collect_types (types_t& typesOut);

private:
  type_name_map_t m_typeNameMap;
  type_hash_map_t m_typeHashMap;
};

}// end of namespace

#include <moose/types.i>
