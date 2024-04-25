// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2022 Sebastian Reiter <s.b.reiter@gmail.com>
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

#include <array>
#include <cstdint>
#include <string>

#include <moose/export.h>

namespace moose
{
  class Version
  {
  public:
    using Values = std::array<uint32_t, 3>;
    using iterator = Values::iterator;

  public:
    MOOSE_EXPORT static auto fromString (std::string_view s) -> Version;

    MOOSE_EXPORT Version ();
    MOOSE_EXPORT Version (uint32_t patch);
    MOOSE_EXPORT Version (uint32_t minor, uint32_t patch);
    MOOSE_EXPORT Version (uint32_t major, uint32_t minor, uint32_t patch);
    
    MOOSE_EXPORT bool operator == (Version const& other) const;
    MOOSE_EXPORT bool operator < (Version const& other) const;

    MOOSE_EXPORT auto toString () const -> std::string;
    
  private:
    Values mValues {0, 0, 0};
  };
}//  end of namespace moose
