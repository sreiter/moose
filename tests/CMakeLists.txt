cmake_minimum_required (VERSION 3.15)

add_executable (
    moose_tests
    enums.t.cpp
    json_archive_in.t.cpp
    stl.t.cpp
    unpacking.t.cpp
    version.t.cpp)

target_compile_features(moose_tests PUBLIC cxx_std_20)

include (FetchContent)
FetchContent_Declare (
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.12.1
    GIT_SHALLOW ON)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable (googletest)

target_link_libraries (moose_tests gtest_main moose)

enable_testing()
add_test(NAME moose_ctest COMMAND moose_tests)
