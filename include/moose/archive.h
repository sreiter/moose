// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <moose/range.h>
#include <moose/type_traits.h>
#include <moose/types.h>

namespace moose
{
  enum class Hint
  {
    None,
    OneLine,
    ChildrenOneLine
  };
    
  class Archive
  {
  public:
    enum class Mode
    {
      Read,
      Write
    };

  public:
    Archive (Mode mode);

    virtual ~Archive ();

    template <class T>
    void operator () (const char* name, T& value, Hint hint = Hint::None);

    /// Read with default value
    template <class T>
    void operator () (const char* name, T& value, const T& defVal, Hint hint = Hint::None);

    bool is_reading () const;
    bool is_writing () const;

  protected:
    /** Returns false if the specified entry was not found.
      If false was returned, no matching end_entry may be called.*/
    virtual bool begin_entry (const char* name, EntryType entryType, Hint hint);
    virtual void end_entry (const char* name, EntryType entryType);

    virtual bool read_array_has_next (const char* name);

    virtual std::string read_type_name ();
    virtual void write_type_name (std::string const& typeName);

  /// reads/writes a number value (int, float, ...).
  /** Default implementation redirects to 'apply (const char*, double&)'
   * \{ */
    virtual void archive (const char* name, bool& val);
    virtual void archive (const char* name, char& val);
    virtual void archive (const char* name, unsigned char& val);
    virtual void archive (const char* name, int& val);
    virtual void archive (const char* name, long int& val);
    virtual void archive (const char* name, long long int& val);
    virtual void archive (const char* name, unsigned int& val);
    virtual void archive (const char* name, unsigned long int& val);
    virtual void archive (const char* name, unsigned long long int& val);
    virtual void archive (const char* name, float& val);
    virtual void archive (const char* name, double& val) = 0;
  /** \} */

    virtual void archive (const char* name, std::string& val) = 0;

  private:
    /// Used to allow for different overloads based on the entryType.
    template <EntryType entryType>
    struct EntryTypeDummy {};

  /** If a concrete type is defined by the current entry in the archive,
    the corresponding Type object is returned. If not, the Type object
    corresponding to the given template argument is returned.
  */
    template <class T>
    Type const& archive_type (T& instance);

    template <class T>
    void archive (const char* name, T& value, EntryTypeDummy <EntryType::Value>);

    template <class T>
    void archive (const char* name, T& value, EntryTypeDummy <EntryType::Struct>);

    template <class T>
    void archive (const char* name, std::shared_ptr<T>& sp, EntryTypeDummy <EntryType::Struct>);

    template <class T>
    void archive (const char* name, std::unique_ptr<T>& up, EntryTypeDummy <EntryType::Struct>);

    template <class T>
    void archive (const char* name, T*& p, EntryTypeDummy <EntryType::Struct>);

    template <class T>
    void archive (const char* name, T& value, EntryTypeDummy <EntryType::Vector>);

    template <class T>
    void archive (const char* name, T& value, EntryTypeDummy <EntryType::Range>);

    template <class T>
    void archive_double(const char* name, T& val);

  private:
    Mode m_mode;
  };
}// end of namespace moose

#include <moose/archive.i>
