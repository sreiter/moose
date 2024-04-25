#pragma once

#ifdef MOOSE_COMPILING_LIBRARY
  #define MOOSE_EXPORT __declspec(dllexport)
#else
  #define MOOSE_EXPORT __declspec(dllimport)
#endif
