// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_exception_util
#define __H__moose_exception_util

#include <sstream>
#include <moose/exceptions.h>

#define MOOSE_THROW(msg)        {std::stringstream ss; ss << msg; throw(std::runtime_error(ss.str()));}
#define MOOSE_CTHROW(cond, msg) {if(cond){std::stringstream ss; ss << msg; throw(std::runtime_error(ss.str()));}}

#define MOOSE_AR_THROW(msg)        {std::stringstream ss; ss << msg; throw(ArchiveError(ss.str()));}
#define MOOSE_AR_CTHROW(cond, msg) {if(cond){std::stringstream ss; ss << msg; throw(ArchiveError(ss.str()));}}

#define MOOSE_FAC_THROW(msg)        {std::stringstream ss; ss << msg; throw(FactoryError(ss.str()));}
#define MOOSE_FAC_CTHROW(cond, msg) {if(cond){std::stringstream ss; ss << msg; throw(FactoryError(ss.str()));}}

#endif  //__H__moose_exception_util
