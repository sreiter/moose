// This file is part of moose, a C++ serialization library
//
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
#include <moose/version.h>

namespace moose
{
  auto Version::fromString (std::string_view s) -> Version
  {
    using namespace std;
    auto const first = s.find_first_of ('.');
    auto const last = s.find_last_of ('.');
    if (first == string::npos ||
        last == string::npos ||
        first >= last)
    {
      assert (!"Invalid version string supplied. Expected is a version string like `3.2.1`.");
      return {};
    }

    try
    {
      uint32_t major, minor, patch;
      major = static_cast <uint32_t> (stoi (std::string {s.substr (0, first)}));
      minor = static_cast <uint32_t> (stoi (std::string {s.substr (first + 1, last - first + 1)}));
      patch = static_cast <uint32_t> (stoi (std::string {s.substr (last + 1, s.size () - last + 1)}));
      return {major, minor, patch};
    }
    catch (...)
    {
      assert (!"Failure while converting string to int. Expected is a version string like `3.2.1`.");
    }

    return {};
  }

  Version::Version () = default;

  Version::Version (uint32_t patch)
    : Version {0, 0, patch}
  {}

  Version::Version (uint32_t minor, uint32_t patch)
    : Version {0, minor, patch}
  {}

  Version::Version (uint32_t major, uint32_t minor, uint32_t patch)
    : mValues {major, minor, patch}
  {}

  bool Version::operator == (Version const& other) const
  {
    return mValues == other.mValues;
  }

  bool Version::operator < (Version const& other) const
  {
    return mValues < other.mValues;
  }

  auto Version::toString () const -> std::string
  {
    using namespace std;
    return to_string (mValues [0]) + "." + to_string (mValues [1]) + "." + to_string (mValues [2]);
  }
}//  end of namespace moose
