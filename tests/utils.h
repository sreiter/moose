#pragma once

#include <moose/from_binary.h>
#include <moose/from_json.h>
#include <moose/to_binary.h>
#include <moose/to_json.h>

template <class T>
T toBinaryAndBack (T const& t)
{
  auto const binary = moose::toBinary (t);
  return moose::fromBinary<T> (binary);
}

template <class T>
T toJsonAndBack (T const& t)
{
  auto const json = moose::toJson ("t", t);
  return moose::fromJson<T> ("t", json.c_str ());
}
