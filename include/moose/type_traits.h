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

#include <string>

namespace moose
{
  enum class EntryType
  {
    Struct,
    Value, ///< Built in values. Todo: Call this `BuiltIn`
    Range,
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
          static ForwardedType const& getForwardedValue (YourWrappingType const& from);
          static void setForwardedValue (YourWrappingType& to, ForwardedType&& value);
        };
      \endcode
    */
    ForwardValue ///< For types which wrap a single value and don't want to use a separate struct
  };

  template <class T>
  struct TypeTraits
  {
    static constexpr EntryType entryType = EntryType::Struct;
  };

  /** Has to be specified for each type of \a EntryType::Vector which
    does not contain a typedef \a value_type.
  */
  template <class T>
  struct VectorTraits
  {
    using ValueType = typename T::value_type;
  };

  /** This method is called for each value which is deserialized as part of
    a \a EntryType::Vector entry.

    Overload it for your custom \a EntryType::Vector type, if it doesn't feature
    a \a push_back(ValueType const&) method.
  */
  template <class Vector, class Value>
  void VectorPushBack (Vector& vector, Value const& value)
  {
    vector.push_back (value);
  }

  /** This method is called for each deserialized \a EntryType::Vector entry.

    Overload it for your custom \a EntryType::Vector type, if it doesn't feature
    a \a clear() method.
  */
  template <class Vector>
  void VectorClear (Vector& vector)
  {
    vector.clear ();
  }

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
}// end of namespace moose
