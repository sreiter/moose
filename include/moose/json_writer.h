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

#include <fstream>
#include <stack>
#include <memory>
#include <moose/writer.h>

namespace moose
{

class JSONWriter : public Writer
{
public:
  static auto toFile (const char* filename) -> std::shared_ptr<JSONWriter>;

  JSONWriter (const char* filename);
  JSONWriter (std::shared_ptr <std::ostream> out);
  JSONWriter (JSONWriter const&) = delete;
  JSONWriter (JSONWriter&& other);

  virtual ~JSONWriter ();

  JSONWriter& operator = (JSONWriter const&) = delete;
  JSONWriter& operator = (JSONWriter&& other);

  bool begin_entry (const char* name, EntryType entryType, Hint hint) override;
  void end_entry (const char* name, EntryType entryType) override;

  void write_type_name (std::string const& typeName) override;
  void write_type_version (Version const& version) override;
  
  void write (const char* name, bool val) override;
  void write (const char* name, double val) override;
  void write (const char* name, std::string const& val) override;

private:
  void prepare_content ();
  void optional_endl ();
  Hint hint () const;
  auto out () -> std::ostream &;

private:
  std::shared_ptr <std::ostream> m_out;
  size_t m_currentDepth {0};
  size_t m_lastWrittenDepth {0};
  std::stack <Hint> m_hints;
};

}// end of namespace moose
