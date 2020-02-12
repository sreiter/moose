// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_exceptions
#define __H__moose_exceptions

#include <stdexcept>

namespace moose {

class ArchiveError : public std::runtime_error
{
public:
  explicit ArchiveError (const std::string& what) :
    std::runtime_error (what) {}
};

class FactoryError : public std::runtime_error
{
public:
  explicit FactoryError (const std::string& what) :
    std::runtime_error (what) {}
};

}// end of namespace moose

#endif  //__H__moose_exceptions