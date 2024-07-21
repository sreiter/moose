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

#include <moose/export.h>
#include <moose/reader.h>

#include <memory>
#include <stack>

namespace moose
{
  class BinaryReader : public Reader {
  public:
    MOOSE_EXPORT static auto fromFile (const char* filename) -> std::shared_ptr<BinaryReader>;

  public:
    BinaryReader () = delete;
    MOOSE_EXPORT BinaryReader (BinaryReader&& other) = default;
    MOOSE_EXPORT BinaryReader (std::istream& in);
    MOOSE_EXPORT BinaryReader (std::shared_ptr<std::istream> in);

    BinaryReader (BinaryReader const&) = delete;

    MOOSE_EXPORT ~BinaryReader () override = default;

    BinaryReader& operator = (BinaryReader const&) = delete;
    MOOSE_EXPORT BinaryReader& operator = (BinaryReader&& other) = default;

    bool begin_entry (const char* name, ContentType type) override;
    void end_entry (const char* name, ContentType type) override;

    bool array_has_next (const char* name) const override;

    auto type_name () const -> std::string override;
    auto type_version () const -> Version override;
    
    void read (const char* name, bool& value) const override;
    void read (const char* name, double& value) const override;
    void read (const char* name, std::string& value) const override;

  private:
    struct Entry
    {
      ContentType mType;
      bool mArrayHasNext {false};
    };

  private:
    auto current () -> Entry&;
    auto current () const -> Entry const&;
    bool readArrayHasNext ();
    auto in () const -> std::istream&;

  private:
    std::shared_ptr<std::istream> mStreamStorage;
    std::istream* mIn;
    std::stack<Entry> mEntries;
  };
}// end of namespace moose
