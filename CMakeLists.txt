# This file is part of moose, a C++ serialization library
#
# Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

cmake_minimum_required (VERSION 3.1)

project (libmoose)

set (mooseSrc   src/moose/archive.cpp
                src/moose/json_archive.cpp)

add_library(moose STATIC ${mooseSrc})

target_include_directories(moose
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/rapidjson-v1.1.0.s1/include
    )

target_compile_features(moose PUBLIC cxx_std_17)

add_subdirectory (sample)
