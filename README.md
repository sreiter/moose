# moose &nbsp;&nbsp;&nbsp;&nbsp; [![Build Status](https://travis-ci.com/sreiter/moose.svg?branch=master)](https://travis-ci.com/sreiter/moose)
## Introduction
**Moose** (**MO**ose **O**bject **SE**rialization) is a *BSD* licensed *C++* library for the serialization of complex data structures.

**Moose** provides functionality to create and populate instances of complex *C++* data structures, e.g., from **.json** files.
By specifying properties and by optionally annotating objects with some keywords (e.g. ```"@type": "SomeDerivedClass"```), users can create human readable
descriptions of complex C++ data structures, including *arrays*, *vectors*, *custom polymorphic classes*, etc.

The general idea of **moose** is thereby similar to _boost serialization_ and similar libraries.
An **Archive** class serves as an abstraction layer between serialization code and concrete file formats.
By either overloading a public

    moose::Serialize (moose::Archive& ar, Object& o)

function for a given ```Object``` type, or by providing a method

    Object::serialize (moose::Archive& ar)

one can add serialization support for custom types.

The implementation of **moose** is kept simple on purpose to allow users of the library to easily create **custom readers/writers**, which may be used to support more file formats.

Another interesting application for **archives** is the generation and synchronization of **GUI** elements given a serializable object.

While already usable, **moose** is not yet a full grown library. Most parts are in an early prototype stage.

## Example
A small sample is provided in **moose/sample/moosesample.cpp**. The generated output shown below may give a rough idea of the general idea behind **moose**.
Please note that ```ClassA``` and ```ClassB``` derive from a common base class ```BaseClass``` and are serialized into an ```std::vector``` of ```std::shared_ptr <BaseClass>```.

    moosesample - reading json input:
    
      {
        "sampleArray": {"0": 100, "1": 101},
        "objects": [
            {"@type": "ClassA", "value": "hello"},
            {"@type": "ClassB", "data": {"0": 102, "1": 103}}
          ]
      }
    
    Output from generated objects:
    simpleArray: 100, 101
    objects:
      hello
      102, 103

## Building moose as a stand-alone project
**moose** features a CMake based build system. Create a 'build' directory in the **moose** root folder and from that build directory execute, e.g.:

    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j

In addition to the **moose** library, the following executable will be build:

    sample/moose_sample

## Building moose as a part of your project
To build **moose** as part of your project, simply add the line
    
    add_subdirectory (moose)

to the CMakeLists.txt of your project. To link it to your target, please use the following line:

    target_link_libraries (YOUR_TARGET  moose)

Necessary include directories will be automatically added to your target.

## License
**moose** is licensed under a *2-clause BSD* license:

    moose (MOose Object SErialization), a C++ serialization library
    
    Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
    Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
       * Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
Please note that **moose** is deployed with the open source library **rapidjson** (MIT license).
