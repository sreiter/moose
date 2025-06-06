# This file is part of moose, a C++ serialization library
#
# Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>
# Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required (VERSION 3.11)

option (MOOSE_BUILD_STATIC "Build the moose library as static library")
option (MOOSE_BUILD_SAMPLE "Build the moose sample application")
option (MOOSE_BUILD_TESTS "Build the moose tests")

project (libmoose)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (mooseSrc   src/moose/archive.cpp
                src/moose/binary_reader.cpp
                src/moose/input_archive.cpp
                src/moose/json_reader.cpp
                src/moose/json_writer.cpp
                src/moose/output_archive.cpp
                src/moose/type.cpp
                src/moose/types.cpp
                src/moose/version.cpp)

if (MOOSE_BUILD_STATIC)
  add_library(moose STATIC ${mooseSrc})
else ()
  add_library(moose SHARED ${mooseSrc})
endif ()

target_include_directories(moose
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/rapidjson-v1.1.0.s1/include
    )

target_compile_features(moose PUBLIC cxx_std_20)

if(MSVC)
  #/wd5054 is a warning from rapidjson.
  target_compile_options(moose PRIVATE /W4 /WX /wd5054)
else()
  target_compile_options(moose PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_compile_definitions (moose PRIVATE MOOSE_COMPILING_LIBRARY)

include (FetchContent)
FetchContent_Declare (
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum
    GIT_TAG v0.9.6
    GIT_SHALLOW ON)
FetchContent_MakeAvailable (magic_enum)

target_link_libraries (moose PUBLIC magic_enum)

if (MOOSE_BUILD_SAMPLE)
  add_subdirectory (sample)
endif ()

if (MOOSE_BUILD_TESTS)
  add_subdirectory (tests)
endif ()
