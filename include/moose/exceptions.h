// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <typeinfo>

namespace moose {

// moose exceptions derive from this class
class Exception : public std::runtime_error
{
public:
  Exception () : std::runtime_error ("") {}

  const char* what () const noexcept override {return m_what.c_str();}

  template <class T>
  Exception& operator << (const T& t)    {what ().append (to_string (t)); return *this;}

  Exception& operator << (const char* t) {what ().append (t); return *this;}

private:
  auto what () -> std::string&
  {
    if (m_what.empty ())
      m_what.append (typeid (*this).name ()).append (": ");
    return m_what;
  }

  template <class T>
  auto to_string (const T& t) const -> std::string
  {
    return std::to_string (t);
  }

  auto to_string (const std::string& s) const -> std::string const&
  {
    return s;
  }

  auto to_string (const char* s) const -> std::string
  {
    return s;
  }

  template <class T>
  auto to_string (const std::vector <T>& v) -> std::string
  {
    std::string s ("{");
    for(size_t i = 0; i < v.size(); ++i) {
        if (i > 0) {
            s.append (", ");
        }
        s.append (to_string (v[i]));
    }
    s.append ("}");
    return s;
  }

private:
  std::string m_what;
};

class ArchiveError : public Exception {};
class FactoryError : public Exception {};

}// end of namespace moose
