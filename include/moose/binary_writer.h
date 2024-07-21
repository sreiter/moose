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

#include <fstream>
#include <stack>
#include <memory>
#include <moose/writer.h>

namespace moose
{

/** Writes binary data to a given stream, using its
  `write (const char_type* s, std::streamsize count)` method.
*/
template <class STREAM = std::ofstream>
class BinaryWriter : public Writer
{
public:
  static auto toFile (const char* filename) -> std::shared_ptr<BinaryWriter>;

  BinaryWriter (STREAM& out);
  BinaryWriter (std::shared_ptr<STREAM> out);
  
  BinaryWriter (BinaryWriter const&) = delete;
  BinaryWriter (BinaryWriter&& other) = default;
  

  ~BinaryWriter () override = default;

  BinaryWriter& operator = (BinaryWriter const&) = delete;
  BinaryWriter& operator = (BinaryWriter&& other) = default;

  bool begin_entry (const char* name, ContentType type, Hint hint) override;
  void end_entry (const char* name, ContentType type) override;

  void write_type_name (std::string const& typeName) override;
  void write_type_version (Version const& version) override;
  
  void write (const char* name, bool value) override;
  void write (const char* name, double value) override;
  void write (const char* name, std::string const& value) override;

private:
  auto out () -> STREAM&;

private:
  std::shared_ptr<STREAM> mStreamStorage;
  STREAM* mOut;
  std::stack<ContentType> mContentStack;
  std::stack<std::string> mNameStack;
};

}// end of namespace moose

#include <moose/binary_writer.i>