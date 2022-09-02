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

#include <moose/archive.h>
#include <moose/exceptions.h>

namespace moose
{
  Archive::Archive (std::shared_ptr<InputArchive> archive)
    : mInput {std::move (archive)}
  {
    if (mInput == nullptr)
      throw ArchiveError () << "A valid input archive is expected, but nullptr was provided.";
  }

  Archive::Archive (std::shared_ptr<OutputArchive> archive)
    : mOutput {std::move (archive)}
  {
    if (mOutput == nullptr)
      throw ArchiveError () << "A valid output archive is expected, but nullptr was provided.";
  }

  bool Archive::is_reading () const
  {
    return mInput != nullptr;
  }

  bool Archive::is_writing () const
  {
    return mOutput != nullptr;
  }

  auto Archive::type_version (Version const& latestVersion) -> Version
  {
    if (is_reading ())
    {
      return mInput->type_version ();
    }
    else
    {
      mOutput->write_type_version (latestVersion);
      return latestVersion;
    }
  }

  bool Archive::begin_entry (const char* name, EntryType entryType, Hint hint)
  {
    if (is_reading ())
      return mInput->begin_entry (name, entryType);
    else
      return mOutput->begin_entry (name, entryType, hint);
  }

  void Archive::end_entry (const char* name, EntryType entryType)
  {
    if (is_reading ())
      mInput->end_entry (name, entryType);
    else
      mOutput->end_entry (name, entryType);
  }
}// end of namespace moose
