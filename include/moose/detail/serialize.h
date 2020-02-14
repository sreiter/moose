// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#pragma once

#include <string>
#include <stdexcept>
#include <typeinfo>

namespace moose
{
  class Archive;
}

namespace moose::detail
{

template<typename C>
struct has_serialize {
private:
    template<typename T>
    static auto check()
    -> typename
        std::is_same<
            decltype( std::declval<T>().serialize( std::declval<Archive>()) ),
            void    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        >::type;  // attempt to call it and see if the return type is correct

    template<typename>
    static std::false_type check(...);

    typedef decltype(check<C>()) type;

public:
    static const bool value = type::value;
};

template <class T>
typename std::enable_if<has_serialize<T>::value >::type
Serialize(Archive& ar, T& val)
{
  val.serialize (ar);
}


template <class T>
typename std::enable_if<!has_serialize<T>::value >::type
Serialize(Archive& ar, T& val)
{
//todo: Make this a compile-time error.
    using namespace std;
    throw runtime_error(string("No serialization method exists for '")
                        .append(typeid(T).name()).append("'"));
}

}// end of namespace
