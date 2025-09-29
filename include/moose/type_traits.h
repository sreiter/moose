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

#include <moose/exceptions.h>
#include <moose/hint.h>
#include <moose/range.h>

#include <magic_enum.hpp>

#include <string>

namespace moose
{
  enum class EntryType
  {
    Struct,

    Value, ///< Plain values. Those are directly supported by the archive.

    /** A fixed size range of entries. Implementing the method `toRange` is required to provide
      a pair of begin/end iterators which are used to iterate over the members of the range.
      If a range consists of entries with type `Value`, then the contents of that `range` can
      be unpacked into its parent `vector`, if the traits specify the constant `canBeUnpacked`:
      \code
        template <>
        struct TypeTraits<YourChildRange>
        {
          static constexpr EntryType entryType = EntryType::Range;
          static constexpr bool canBeUnpacked = true;
          static auto toRange (YourRangeType& v) -> Range <SomeIteratorType>;
        };
      \endcode
    */
    Range,

    /** A dynamic sequence of values. Think of a `std::vector`. The following typetraits has to
      be specified for it to be compatible with the archive.
      Implementing the method `toRange` is required to provide a pair of begin/end iterators
      which are used to iterate over the members of the range.
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

      Child ranges may be unpacked by a vector if the following constant is additionally specified:
      \code
        template <>
        struct TypeTraits<YourParentRange>
        {
          ...
          static constexpr bool wantsToUnpack = true;
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
    static constexpr Hint hint = Hint::None;
  };

  template <class T>
  struct DefaultRangeTraits
  {
    static constexpr EntryType entryType = EntryType::Range;
    static auto toRange (T& container)
    {
      return makeRange (container.begin (), container.end ());
    }
  };
  
  template <class T>
  constexpr bool isValue ()
  { return TypeTraits<T>::entryType == EntryType::Value; }

  template <class T>
  constexpr bool isStruct ()
  { return TypeTraits<T>::entryType == EntryType::Struct; }

  template <class T>
  constexpr bool isRange ()
  { return TypeTraits<T>::entryType == EntryType::Range; }

  template <class T>
  constexpr bool isVector ()
  { return TypeTraits<T>::entryType == EntryType::Vector; }

  template <class T>
  constexpr bool isForwardValue ()
  { return TypeTraits<T>::entryType == EntryType::ForwardValue; }

  template <class T>
  constexpr bool isForwardReference ()
  { return TypeTraits<T>::entryType == EntryType::ForwardReference; }

  template <class T>
  concept TraitsHas_canBeUnpacked = requires ()
  { {TypeTraits<T>::canBeUnpacked} -> std::convertible_to<bool>; };

  template <TraitsHas_canBeUnpacked T>
  constexpr bool canBeUnpacked ()
  {
    return isRange<T> () && TypeTraits<T>::canBeUnpacked;
  }

  template <class T>
  constexpr bool canBeUnpacked ()
  {
    return false;
  }

  template <class T>
  concept TraitsHas_wantsToUnpack = requires ()
  { {TypeTraits<T>::wantsToUnpack} -> std::convertible_to<bool>; };

  template <TraitsHas_wantsToUnpack T>
  constexpr bool wantsToUnpack ()
  {
    return isVector<T> () && TypeTraits<T>::wantsToUnpack;
  }

  template <class T>
  constexpr bool wantsToUnpack ()
  {
    return false;
  }

  template <class T>
  concept TraitsHas_hint = requires ()
  { {TypeTraits<T>::hint} -> std::convertible_to<Hint>; };

  template <TraitsHas_hint T>
  auto getDefaultHint (T const&) -> Hint
  { return TypeTraits<T>::hint; }
  
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

    \warning The integer constants of the enum class should never change, at least not for
             serialized constants.
  */
  template <class T>
  struct EnumToIntTraits
  {
    static constexpr EntryType entryType = EntryType::ForwardValue;
    using ForwardedType = std::underlying_type<T>;

    static ForwardedType getForwardedValue (T const& from)
    {
      return static_cast<ForwardedType> (from);
    }

    static void setForwardedValue (T& to, ForwardedType value)
    {
      to = static_cast<T> (value);
    }
  };

  /** Converts enums to string and back, on the fly during serialization.
    Internally, the `magic_enum` library is used. Through traits, the enum to string
    conversion can be tuned to your needs. Please see the `magic_enum` documentation
    for more information.

    \warning By default, magic_enum only supports values between -128 and 127. This can be adjusted
             through type traits.
             See https://github.com/Neargye/magic_enum/blob/master/doc/limitations.md

    \note If your enum class consists of flags, you may want to look into the is_flags specialization,
          See https://github.com/Neargye/magic_enum/blob/master/doc/limitations.md
  */
  template <class T>
  struct EnumToStringTraits
  {
    static constexpr EntryType entryType = EntryType::ForwardValue;
    using ForwardedType = std::string;

    static std::string getForwardedValue (T const& from)
    {
      return std::string {magic_enum::enum_name (from)};
    }

    static void setForwardedValue (T& to, std::string value)
    {
      if (auto const casted = magic_enum::enum_cast<T> (std::string_view {value}))
        to = *casted;
      else
        throw TypeError {} << "Could not translate enum value.";
    }
  };
}// end of namespace moose
