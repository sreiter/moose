# This file is part of moose, a C++ serialization library
#
# Copyright (C) 2020 Sebastian Reiter <s.b.reiter@gmail.com>

cmake_minimum_required (VERSION 3.1)

project (moosesample)

set (src moosesample.cpp)

add_executable(moosesample ${src})

target_compile_features(moosesample PUBLIC cxx_std_17)

target_link_libraries(moosesample moose)
