#pragma once

#ifdef _WIN32
  #ifdef MOOSE_COMPILING_LIBRARY
    #define MOOSE_EXPORT __declspec(dllexport)
  #else
    #define MOOSE_EXPORT __declspec(dllimport)
  #endif
#else
  #define MOOSE_EXPORT
#endif
