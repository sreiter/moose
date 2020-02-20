// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#pragma once

#include <map>
#include <memory>
#include <string>
#include <moose/types.h>

namespace moose{

class Archive {
public:
  Archive (bool reading);

  virtual ~Archive ();

  template <class T>
  void operator () (const char* name, T& value);

  /// Read with default value
  template <class T>
  void operator () (const char* name, T& value, const T& defVal);

protected:
  virtual void begin_read (const char* name);
  virtual void end_read (const char* name);

  virtual void begin_array_read (const char* name);
  virtual bool array_has_next (const char* name) = 0;
  virtual void end_array_read (const char* name);

  virtual std::string get_type_name () = 0;

/// reads a number value (int, float, ...).
/** Default implementation redirects to 'read(const char*, double&)'
 * \{ */
  virtual void read (const char* name, char& val);
  virtual void read (const char* name, unsigned char& val);
  virtual void read (const char* name, int& val);
  virtual void read (const char* name, long int& val);
  virtual void read (const char* name, long long int& val);
  virtual void read (const char* name, unsigned int& val);
  virtual void read (const char* name, unsigned long int& val);
  virtual void read (const char* name, unsigned long long int& val);
  virtual void read (const char* name, float& val);
  virtual void read (const char* name, double& val) = 0;
/** \} */

  virtual void read (const char* name, std::string& val) = 0;

protected:
  template <class T>
  void read (const char* name, T& value);

  template <class T>
  void read (const char* name, std::shared_ptr<T>& sp);

  template <class T>
  void read (const char* name, std::unique_ptr<T>& up);

  template <class T>
  void read (const char* name, T*& p);

  template <class T>
  void read (const char* name, std::vector<T>& value);

private:
  template <class T>
  void read_double(const char* name, T& val);

  bool m_reading;
};

}// end of namespace moose

#include <moose/archive.i>
