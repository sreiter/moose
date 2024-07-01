// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2024 Volume Graphics
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

#include <moose/binary_writer.h>
#include <moose/exceptions.h>

namespace moose
{
  template <class STREAM>
  auto BinaryWriter<STREAM>::toFile (const char* filename) -> std::shared_ptr<BinaryWriter>
  {
    auto out = std::make_shared<std::ofstream> (filename, std::ios::out | std::ios::binary);
    if (!(*out)) throw ArchiveError () << "File not accessible: " << filename;
    return std::make_shared<BinaryWriter> (std::move (out));
  }

  template <class STREAM>
  BinaryWriter<STREAM>::BinaryWriter (std::shared_ptr<STREAM> out)
    : mOut {std::move (out)}
  {
    if (mOut == nullptr)
      throw ArchiveError () << "Invalid stream specified for writing.";
    mContentStack.push (ContentType::Struct);
  }

  template <class STREAM>
  bool BinaryWriter<STREAM>::begin_entry (const char*, ContentType type, Hint)
  {
    if (mContentStack.top () == ContentType::Array)
    {
      char constexpr arrayElement = 1;
      out ().write (&arrayElement, 1); // add marker that an array element follows
    }
    mContentStack.push (type);
    return true;
  }

  template <class STREAM>
  void BinaryWriter<STREAM>::end_entry (const char*, ContentType type)
  {
    if (type == ContentType::Array)
    {
      char constexpr noArrayElement = 0;
      out ().write (&noArrayElement, 1); // add marker that the array is done
    }
    mContentStack.pop ();
  }

  template <class STREAM>
  void BinaryWriter<STREAM>::write_type_name (std::string const& name)
  {
    write ("", name);
  }

  template <class STREAM>
  void BinaryWriter<STREAM>::write_type_version (Version const& version)
  {
    write ("", version.toString ());
  }
  
  template <class STREAM>
  void BinaryWriter<STREAM>::write (const char*, bool value)
  {
    auto const charValue = static_cast<char> (value);
    out ().write (&charValue, 1);
  }

  template <class STREAM>
  void BinaryWriter<STREAM>::write (const char*, double value)
  {
    out ().write (reinterpret_cast<char*> (&value), sizeof (double));
  }

  template <class STREAM>
  void BinaryWriter<STREAM>::write (const char*, std::string const& value)
  {
    auto size = static_cast<uint32_t> (value.size ());
    out ().write (reinterpret_cast<char*> (&size), sizeof (uint32_t));
    out ().write (value.data (), size);
  }

  template <class STREAM>
  auto BinaryWriter<STREAM>::out () -> STREAM&
  {
    return *mOut;
  }
}// end of namespace moose
