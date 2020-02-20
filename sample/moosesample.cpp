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

#include <array>
#include <iostream>
#include <moose/moose.h>

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
    moose::Types::add_without_serialize <BaseClass> ("BaseClass");
    moose::Types::add <ClassA, BaseClass> ("ClassA");
    moose::Types::add <ClassB, BaseClass> ("ClassB");

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
