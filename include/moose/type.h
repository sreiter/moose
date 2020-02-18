// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace moose
{

class Archive;

class Type
{
public:
  using make_raw_fnc_t  = void* (*)();
  using serialize_fnc_t = void (*)(Archive&, void*);

  Type (std::string name,
        std::vector <std::shared_ptr <Type>> baseClasses,
        make_raw_fnc_t makeRawFnc,
        serialize_fnc_t serializeFnc);

  auto name () const -> std::string const&;

  /** If the method returns `true` the type is abstract, i.e.,
    no instances can be created through `make_raw/shared/unique`.
    Returns `false` if the type is not abstract, i.e., `make_raw/shared/unique`
    may be called to create instances of the type.*/
  bool is_abstract () const;

  template <class Base>
  Base* make_raw () const;

  template <class Base>
  std::shared_ptr <Base> make_shared () const;

  template <class Base>
  std::unique_ptr <Base> make_unique () const;

  template <class Base>
  void serialize (Archive& ar, Base& b) const;

  template <class Base>
  bool has_base_class () const;

  bool has_base_class (std::string const& name) const;

private:
  template <class Base>
  void throw_on_bad_base_class (const char* what) const;

private:
  std::string         m_name;
  std::vector <std::shared_ptr <Type>> m_baseClasses;
  make_raw_fnc_t      m_makeRawFnc;
  serialize_fnc_t     m_serializeFnc;
};

}// end of namespace

#include <moose/type.i>
