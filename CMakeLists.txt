# This file is part of moose, a C++ serialization library
#
# Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>



cmake_minimum_required (VERSION 3.1)

project (libmoose)

set (mooseSrc		archive.cpp
     				json_archive.cpp)

add_library(moose ${mooseSrc})
set_property(TARGET moose PROPERTY CXX_STANDARD 14)
target_link_libraries(moose)
