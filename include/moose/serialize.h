// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_serialize
#define __H__moose_serialize

#include <string>
#include <stdexcept>
#include <typeinfo>

namespace moose{

class Archive;

template<typename, typename T>
struct has_serialize {
    static_assert(
        std::integral_constant<T, false>::value,
        "Second template parameter needs to be of function type.");
};

template<typename C, typename Ret, typename... Args>
struct has_serialize<C, Ret(Args...)> {
private:
    template<typename T>
    static auto check(T*)
    -> typename
        std::is_same<
            decltype( std::declval<T>().serialize( std::declval<Args>()... ) ),
            Ret    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        >::type;  // attempt to call it and see if the return type is correct

    template<typename>
    static std::false_type check(...);

    typedef decltype(check<C>(0)) type;

public:
    static const bool value = type::value;
};

template <class T>
typename std::enable_if<has_serialize<T, void(Archive&)>::value >::type
Serialize(Archive& ar, T& val)
{
  val.serialize (ar);
}


template <class T>
typename std::enable_if<!has_serialize<T, void(Archive&)>::value >::type
Serialize(Archive& ar, T& val)
{
//todo: Make this a compile-time error.
    using namespace std;
    throw runtime_error(string("No serialization method exists for '")
                        .append(typeid(T).name()).append("'"));
}

}// end of namespace moose

#endif  //__H__moose_serialize
