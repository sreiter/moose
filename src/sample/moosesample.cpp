// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

#include <array>
#include <iostream>
#include <moose/json_archive.h>

auto const jsonInputData = R"""(
  {
    "sampleArray": {"0": 100, "1": 101},
    "objects": [
        {"@type": "ClassA", "value": "hello"},
        {"@type": "ClassB", "data": {"0": 100, "1": 101}}
      ]
  })""";

namespace moose{
  inline void Serialize (Archive& ar, std::array <int, 2>& v)
  {
    ar ("0", v[0]);
    ar ("1", v[1]);
  }
}

class BaseClass {
  public:
    virtual ~BaseClass () = default;
};

class ClassA : public BaseClass {
  public:
    void serialize (moose::Archive& ar)
    {
      ar("value", m_value);
    }

  private:
    std::string m_value;
};

class ClassB : public BaseClass {
  public:
    void serialize (moose::Archive& ar)
    {
      ar("data", m_data);
    }
  private:
    std::array <int, 2> m_data;
};

int main (int, char**)
{
  std::cout << "moosesample" << std::endl;

  try
  {
    moose::RegisterType <BaseClass> ("BaseClass");
    moose::RegisterType <ClassA, BaseClass> ("ClassA");
    moose::RegisterType <ClassB, BaseClass> ("ClassB");

    auto archive = moose::JSONArchive::fromString (jsonInputData);

    std::array <int, 2> iarray2;
    archive ("sampleArray", iarray2);

    std::vector <std::shared_ptr <BaseClass>> objects;
    archive ("objects", objects);
  }

  catch (std::runtime_error& e)
  {
    std::cout << "An error occurred: " << e.what () << std::endl;
    return 1;
  }

  catch (...)
  {
    std::cout << "An unknown error occurred\n";
    return 1;
  }

  return 0;
}
