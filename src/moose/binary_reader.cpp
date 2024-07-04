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
#include <moose/binary_reader.h>
#include <moose/exceptions.h>
#include <moose/detail/forward_if_not_nullptr.h>

#include <fstream>

namespace moose
{
  auto BinaryReader::fromFile (const char* filename) -> std::shared_ptr<BinaryReader>
  {
    auto in = std::make_shared<std::ifstream> (filename, std::ios::binary);
    if (!(*in))
      throw ArchiveError () << "File not found: " << filename;
    return std::make_shared<BinaryReader> (std::move (in));
  }

  BinaryReader::BinaryReader (std::istream& in)
    : mIn {&in}
  {
    mEntries.emplace (ContentType::Struct, false);
  }

  BinaryReader::BinaryReader (std::shared_ptr<std::istream> in)
    : mStreamStorage {detail::forwardIfNotNullptr<ArchiveError> (std::move (in), "Invalid stream provided")}
    , mIn {mStreamStorage.get ()}
  {
    mEntries.emplace (ContentType::Struct, false);
  }

  bool BinaryReader::begin_entry (const char*, ContentType type)
  {
    if (type == ContentType::Array)
    {
      mEntries.emplace (type, readArrayHasNext ());
      return true;
    }

    mEntries.emplace (type, false);
    return true;
  }

  void BinaryReader::end_entry (const char*, ContentType)
  {
    if (mEntries.empty ())
      throw ArchiveError {} << "`end_entry` called without corresponding `begin_entry`.";

    mEntries.pop ();
    if (mEntries.empty ())
      throw ArchiveError {} << "`end_entry` called without corresponding `begin_entry`.";

    auto& top = mEntries.top ();
    if (top.mType == ContentType::Array)
      top.mArrayHasNext = readArrayHasNext ();
  }

  bool BinaryReader::array_has_next (const char*) const
  {
    return current ().mArrayHasNext;
  }

  auto BinaryReader::type_name () const -> std::string
  {
    std::string name;
    read ("", name);
    return name;
  }

  auto BinaryReader::type_version () const -> Version
  {
    std::string version;
    read ("", version);
    return Version::fromString (version);
  }

  void BinaryReader::read (const char*, bool& value) const
  {
    char charValue;
    in ().read (&charValue, 1);
    value = charValue != 0;
  }

  void BinaryReader::read (const char*, double& value) const
  {
    in ().read (reinterpret_cast<char*> (&value), sizeof (double));
  }

  void BinaryReader::read (const char*, std::string& value) const
  {
    uint32_t size;
    in ().read (reinterpret_cast<char*> (&size), sizeof (uint32_t));
    value.resize (size + 1);
    value.back () = 0;
    in ().read (value.data (), size);
  }

  auto BinaryReader::current () -> Entry&
  {
    return const_cast<Entry&> (const_cast<BinaryReader const*> (this)->current ());
  }

  auto BinaryReader::current () const -> Entry const&
  {
    if (mEntries.empty ())
      throw ArchiveError {} << "Invalid entry queried.";
    return mEntries.top ();
  }

  bool BinaryReader::readArrayHasNext ()
  {
    char hasNext;
    in ().read (&hasNext, 1);
    return hasNext != 0;
  }

  auto BinaryReader::in () const -> std::istream&
  {
    return *mIn;
  }
}// end of namespace moose
