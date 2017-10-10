// This file is part of moose, a C++ serialization library
//
// Copyright (C) 2017 Sebastian Reiter, G-CSC Frankfurt <s.b.reiter@gmail.com>

#ifndef __H__moose_error
#define __H__moose_error

#include <sstream>
#include <stdexcept>

#define MOOSE_THROW(msg)		{std::stringstream ss; ss << msg; throw(std::runtime_error(ss.str()));}
#define MOOSE_CTHROW(cond, msg)	{if(cond){std::stringstream ss; ss << msg; throw(std::runtime_error(ss.str()));}}

#endif	//__H__moose_error
