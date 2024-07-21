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

#include <moose/hint.h>
#include <moose/range.h>

#include <string>

namespace moose
{
  enum class EntryType
  {
    Struct,

    Value, ///< Built in values.

    /** A fixed range of values. Typetraits have to define the following members:
      \code
        template <>
        struct TypeTraits <YourRangeType>
        {
          static constexpr EntryType entryType = EntryType::Range;
          static auto toRange (YourRangeType& v) -> Range <SomeIteratorType>;
        };
      \endcode
    */
    Range,

    /** A dynamic sequence of values. Think of a `std::vector`. The following typetraits has to
      be specified for it to be compatible with the archive.
      \code
        template <>
        struct TypeTraits <YourVectorType>
        {
          static constexpr EntryType entryType = EntryType::Vector;
          using Type = YourVectorType;
          using ValueType = typename Type::value_type;
          static auto toRange (YourVectorType& v) -> Range <SomeIteratorType>;
          static void pushBack (Type& vector, ValueType const& value);
          static void clear (Type& vector);
        };
      \endcode
    */

    Vector,
    /** Types which just wrap a single value of a different type may not need a custom entry. Instead
      it may be convenient to just forward the wrapped value for serialization.
      For such type, the TypeTraits have to specify the following:
      \code
        template <>
        struct TypeTraits <YourWrappingType>
        {
          static constexpr EntryType entryType = EntryType::ForwardValue;
          using ForwardedType = YourForwardedType;
          static ForwardedType getForwardedValue (YourWrappingType const& from);
          static void setForwardedValue (YourWrappingType& to, ForwardedType&& value);
        };
      \endcode
    */

    ForwardValue,

    /** Just like `ForwardValue`, but `getForwardedValue` returns a reference.*/
    ForwardReference,
  };

  template <class T>
  struct TypeTraits
  {
    /** Required entry type. This determines how the archive proceeds with a given type.
      The default traits default to `EntryType::Struct`. I.e., a `serialize` member method
      or a global `Serialize` method for the given type is expected.
    */
    static constexpr EntryType entryType = EntryType::Struct;
  };

  template <class T>
  struct DefaultRangeTraits
  {
    static constexpr EntryType entryType = EntryType::Range;
    static auto toRange (T& container) -> Range <decltype (container.begin ())>
    {
      return {container.begin (), container.end ()};
    }
  };

  /** Overload this method for your types to specify a custom default hint which will be used
    during serialization, if a user didn't specify a hint in the archive call.
  */
  template <class T>
  auto getDefaultHint (T const&) -> Hint
  { return Hint::None; }

  template <>
  struct TypeTraits <bool>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <char>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <unsigned char>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <long int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <long long int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <unsigned int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <unsigned long int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <unsigned long long int>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <float>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <double>
  {static constexpr EntryType entryType = EntryType::Value;};

  template <>
  struct TypeTraits <std::string>
  {static constexpr EntryType entryType = EntryType::Value;};

  /** Convenience class from which type traits for enum class types may derive, if they
    should be converted to `int` and back during serialization/deserialization.

    \warning The integer constants of the enum class should not change, at least not for
             serialized constants.
  */
  template <class T>
  struct EnumClassToIntTypeTraits
  {
    static constexpr EntryType entryType = EntryType::ForwardValue;
    using ForwardedType = int;

    static int getForwardedValue (T const& from)
    {
      return static_cast<int> (from);
    }

    static void setForwardedValue (T& to, int value)
    {
      to = static_cast<T> (value);
    }
  };
}// end of namespace moose
