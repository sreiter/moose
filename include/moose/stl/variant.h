// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
// Copyright (C) 2020-2023 Sebastian Reiter <s.b.reiter@gmail.com>
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

#include <moose/archive.h>

#include <variant>

namespace moose
{
  namespace detail
  {
    template <size_t SIZE>
    struct Variant
    {
      template <class VARIANT>
      static void serialize (Archive& archive, VARIANT& v, size_t index)
      {
        if (index == (SIZE - 1))
        {
          if (archive.is_writing ())
          {
            archive ("value", std::get<SIZE - 1> (v));
          }
          else
          {
            std::variant_alternative_t<SIZE - 1, VARIANT> tmp;
            archive ("value", tmp);
            v = std::move (tmp);
          }
        }
        else
          Variant<SIZE - 1>::serialize (archive, v, index);
      }
    };

    template <>
    struct Variant <0>
    {
      template <class VARIANT>
      static void serialize (Archive&, VARIANT&, size_t)
      {
        throw ArchiveError {} << "Invalid variant index provided.";
      }
    };
  }

  inline void Serialize (Archive&, std::monostate&)
  {
    return;
  }

  template <class... T>
  void Serialize (Archive& archive, std::variant<T...>& v)
  {
    static auto constexpr size = std::variant_size_v<std::variant<T...>>;
    auto index = v.index ();
    archive ("index", index);
    detail::Variant<size>::serialize (archive, v, index);
  }
}// end of namespace
