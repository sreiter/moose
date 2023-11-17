#pragma once

#include <moose/to_json.h>
#include <moose/from_json.h>

// #include <iostream>

template <class T>
T toJsonAndBack (T const& t)
{
  auto const json = moose::toJson ("t", t);
  // std::cout << json << std::endl;
  return moose::fromJson<T> ("t", json.c_str ());
}
