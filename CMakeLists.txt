cmake_minimum_required(VERSION 3.24)
project(cppdiag CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    cpputil
    GIT_REPOSITORY https://github.com/aattoa/cpputil.git
    GIT_TAG        dabf5399fa82be60d695f006058881b1d228cb4d)
set(CPPUTIL_BUILD_TESTS OFF)
FetchContent_MakeAvailable(cpputil)

add_library(cppdiag STATIC)
target_sources(cppdiag
    PRIVATE cppdiag/cppdiag.cpp
    PRIVATE cppdiag/cppdiag.hpp
    PRIVATE cppdiag/format.cpp
    PRIVATE cppdiag/internals.cpp
    PRIVATE cppdiag/internals.hpp)
target_link_libraries(cppdiag PRIVATE cpputil)
target_include_directories(cppdiag PUBLIC cppdiag)

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
