cmake_minimum_required(VERSION 3.24)
project(cppdiag CXX)

set(CMAKE_CXX_STANDARD            23)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)
set(CMAKE_CXX_EXTENSIONS          OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cppdiag
    src/cppdiag/cppdiag.cpp
    src/cppdiag/cppdiag.hpp
    src/cppdiag/format.cpp
    src/cppdiag/internals.cpp
    src/cppdiag/internals.hpp)
target_include_directories(cppdiag PUBLIC src)

include(FetchContent)
FetchContent_Declare(
    cpputil
    GIT_REPOSITORY https://github.com/aattoa/cpputil.git
    GIT_TAG        0ee8e43852cb5aa20803f59529ff7e1a4cbc09ae)
set(CPPUTIL_BUILD_TESTS OFF)
FetchContent_MakeAvailable(cpputil)
target_link_libraries(cppdiag PRIVATE cpputil::util)

if (MSVC)
    target_compile_options(cppdiag PRIVATE "/W4")
else ()
    target_compile_options(cppdiag PRIVATE "-Wall" "-Wextra" "-Wpedantic")
endif ()

option(CPPDIAG_BUILD_TESTS "Build cppdiag tests" OFF)
if (${CPPDIAG_BUILD_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif ()
