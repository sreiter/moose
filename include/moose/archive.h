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
  Archive (bool reading) :
    m_reading (reading)
  {}

  virtual ~Archive () {}

  template <class T>
  void operator () (const char* name, T& value)
  {
    if(m_reading){
      begin_read (name);
      read (name, value);
      end_read (name);
    }
    else{
      // write (name, value);
    }
  }

  /// Read with default value
  template <class T>
  void operator () (const char* name, T& value, const T& defVal)
  {
    try{begin_read (name);}
    catch(ArchiveError&){
      value = defVal;
      return;
    }

    try{read (name, value);}
    catch(ArchiveError&){value = defVal;}

    end_read (name);
  }

protected:
  virtual void begin_read (const char* name) {};
  virtual void end_read (const char* name) {};

  virtual void begin_array_read (const char* name) {};
  virtual bool array_has_next (const char* name) = 0;
  virtual void end_array_read (const char* name) {};

  template <class T>
  void read (const char* name, T& value)
  {
  ///todo: check for POD types and raise an error (->unsupported POD type)
    Serialize (*this, value);
  }

  template <class T>
  void read (const char* name, std::shared_ptr<T>& sp)
  {
    auto const& type = Types::get (get_type_name ());
    if(sp == nullptr)
      sp = type.make_shared <T> ();

    type.serialize (*this, *sp);
  }

  template <class T>
  void read (const char* name, std::unique_ptr<T>& up)
  {
    auto const& type = Types::get (get_type_name ());
    if(up == nullptr)
      up = type.make_unique <T> ();

    type.serialize (*this, *up);
  }

  template <class T>
  void read (const char* name, T*& p)
  {
    auto const& type = Types::get (get_type_name ());
    if(!p)
      p = type.make_raw <T> ();
    type.serialize (*this, *p);
  }


  template <class T>
  void read (const char* name, std::vector<T>& value)
  {
    begin_array_read (name);
    while(array_has_next (name)) {
      T tmpVal;
      (*this) ("", tmpVal);
      value.push_back(tmpVal);
    }
    end_array_read (name);
  }

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


private:
  template <class T>
  void read_double(const char* name, T& val);

  bool m_reading;
};

}// end of namespace moose
