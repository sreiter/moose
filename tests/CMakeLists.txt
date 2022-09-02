cmake_minimum_required (VERSION 3.15)

add_executable (
    moose_tests
    json_archive_in.t.cpp
    version.t.cpp)

include (FetchContent)
FetchContent_Declare (
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.12.1
    GIT_SHALLOW ON)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable (googletest)

target_link_libraries (moose_tests gtest_main moose)
