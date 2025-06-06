#pragma once

#if defined MOOSE_DYNAMIC_LIBRARY and defined _WIN32
  #ifdef MOOSE_COMPILING_LIBRARY
    #define MOOSE_EXPORT __declspec(dllexport)
  #else
    #define MOOSE_EXPORT __declspec(dllimport)
  #endif
#else
  #define MOOSE_EXPORT
#endif
