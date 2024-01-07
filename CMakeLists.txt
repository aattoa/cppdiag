cmake_minimum_required(VERSION 3.24)
project(cppdiag CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cppdiag STATIC)
target_sources(cppdiag
    PRIVATE cppdiag/cppdiag.cpp
    PRIVATE cppdiag/cppdiag.hpp
    PRIVATE cppdiag/format.cpp
    PRIVATE cppdiag/internals.cpp
    PRIVATE cppdiag/internals.hpp)
target_include_directories(cppdiag
    PUBLIC cppdiag)

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
