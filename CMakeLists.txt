cmake_minimum_required(VERSION 3.24)
project(cppdiag CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/cppdiag.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/cppdiag.hpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/format.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/internals.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/internals.hpp)
target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME})

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4")
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic")
endif ()

option(CPPDIAG_BUILD_TESTS "Build cppdiag tests" OFF)
if (${CPPDIAG_BUILD_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif ()
