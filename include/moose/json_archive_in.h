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

#include <stack>
#include <memory>
#include <moose/archive.h>

namespace moose
{

class JSONArchiveIn : public Archive {
public:
  static JSONArchiveIn fromFile (const char* filename);
  static JSONArchiveIn fromString (const char* str);

public:
  JSONArchiveIn ();
  JSONArchiveIn (JSONArchiveIn const&) = delete;
  JSONArchiveIn (JSONArchiveIn&& other);

  virtual ~JSONArchiveIn () = default;

  JSONArchiveIn& operator = (JSONArchiveIn const&) = delete;
  JSONArchiveIn& operator = (JSONArchiveIn&& other);

  void parse_file (const char* filename);
  void parse_stream (std::istream& in);
  void parse_string (const char* str);

protected:
  bool begin_entry (const char* name, EntryType entryType, Hint hint) override;
  void end_entry (const char* name, EntryType entryType) override;

  bool read_array_has_next (const char* name) override;

  std::string read_type_name () override;

  auto read_type_version () -> Version override;
  void write_type_version (Version const& version) override;
  
  void archive (const char* name, double& val) override;
  void archive (const char* name, std::string& val) override;

private:
  struct ParseData;
  std::shared_ptr <ParseData> m_parseData;
};

}// end of namespace moose
