// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020-2022 Sebastian Reiter <s.b.reiter@gmail.com>
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

#include <memory>
#include <string>
#include <moose/export.h>
#include <moose/range.h>
#include <moose/type_traits.h>
#include <moose/version.h>
#include <moose/reader.h>
#include <moose/writer.h>

namespace moose
{
  class Type;

  /** \brief An archive yields either read or write access to an underlying data stream.
  */
  class Archive
  {
  public:
    MOOSE_EXPORT Archive (std::shared_ptr<Reader> archive);
    MOOSE_EXPORT Archive (std::shared_ptr<Writer> archive);

    /** For output archives, latestVersion is stored as version of the currently processed type
      and is also returned.

      For input archives, the type version stored in the underlying archive is returned.
      If no version was stored, {0,0,0} is returned.
    */
    MOOSE_EXPORT auto type_version (Version const& latestVersion) -> Version;

    /** \brief Read/write without default value.
      If the archive is reading and the given name can not be found, an exception is thrown.
    */
    template <class T>
    void operator () (const char* name, T& value, Hint hint = Hint::None);

    /** \brief Read/write with default value
      If the archive is reading and the given name can not be found, the default value is returned.
    */
    template <class T>
    void operator () (const char* name, T& value, const T& defVal, Hint hint = Hint::None);

    /// Write a const value
    template <class T>
    void operator () (const char* name, T const& value, Hint hint = Hint::None);

    /** \brief Read/write without name and without default value.
      A dummy name is generated on the fly for writing and reading archives. It is thus crucial that
      the reading and writing order for unnamed values is consistent. Otherwise, an exception is thrown.
    */
    template <class T>
    void operator () (T& value, Hint hint = Hint::None);

    /** \brief Read/write without name but with default value.
      A dummy name is generated on the fly for writing and reading archives. It is thus crucial that
      the reading and writing order for unnamed values is consistent. Otherwise, an exception is thrown.
    */
    template <class T>
    void operator () (T& value, const T& defVal, Hint hint = Hint::None);

    /// Write an unnamed const value. See the non-const overload for more information.
    template <class T>
    void operator () (T const& value, Hint hint = Hint::None);

    MOOSE_EXPORT bool is_reading () const;
    MOOSE_EXPORT bool is_writing () const;

  private:
    MOOSE_EXPORT bool begin_entry (const char* name, ContentType contentType, Hint hint);
    MOOSE_EXPORT void end_entry (const char* name, ContentType contentType);

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
    void archive (const char* name, T& value, EntryTypeDummy <EntryType::ForwardValue>);

    template <class T>
    void archive (const char* name, T& value, EntryTypeDummy <EntryType::ForwardReference>);

    template <class T>
    void archive (const char* name, T& value);
    
    template <class TRAITS>
    auto contentType (TRAITS const&, EntryTypeDummy<EntryType::Struct>) -> ContentType;

    template <class TRAITS>
    auto contentType (TRAITS const&, EntryTypeDummy<EntryType::Value>) -> ContentType;

    template <class TRAITS>
    auto contentType (TRAITS const&, EntryTypeDummy<EntryType::Range>) -> ContentType;

    template <class TRAITS>
    auto contentType (TRAITS const&, EntryTypeDummy<EntryType::Vector>) -> ContentType;

    template <class TRAITS>
    auto contentType (TRAITS const&, EntryTypeDummy<EntryType::ForwardValue>) -> ContentType;

    template <class TRAITS>
    auto contentType (TRAITS const&, EntryTypeDummy<EntryType::ForwardReference>) -> ContentType;

  private:
    std::shared_ptr<Reader> mInput;
    std::shared_ptr<Writer> mOutput;
  };
}// end of namespace moose

#include <moose/archive.i>
