// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#pragma once

#include <stdexcept>
#include <string>
#include <vector>

/// Declares an exception class. baseClass should derive be moose::Exception or a derived class.
#define DECLARE_CUSTOM_EXCEPTION(className, baseClass) \
  class className : public baseClass {\
  public:\
      className () : baseClass (#className) {}\
  protected:\
      className (const char* derivedClassName) : baseClass (derivedClassName) {}\
  }; 

namespace moose {

// moose exceptions derive from this class
class Exception : public std::runtime_error
{
public:
  Exception ()
    : std::runtime_error ("")
    , m_what ("Exception: ")
  {}

  const char* what () const noexcept override {return m_what.c_str();}

  template <class T>
  Exception& operator << (const T& t)    {m_what.append (to_string (t)); return *this;}
  Exception& operator << (const char* t) {m_what.append (t); return *this;}

protected:
  Exception (const char* derivedClassName)
    : std::runtime_error ("")
    , m_what (derivedClassName)
  {
    m_what.append (": ");
  }

private:
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

DECLARE_CUSTOM_EXCEPTION (ArchiveError, Exception);
DECLARE_CUSTOM_EXCEPTION (FactoryError, Exception);

}// end of namespace moose
