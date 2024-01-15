include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        766541d12d64845f5232a1ce4e34a85e83506b09)
FetchContent_MakeAvailable(Catch2)
include(CTest)
include(Catch)

add_executable(test-cppdiag internals.cpp interface.cpp)
target_include_directories(test-cppdiag PRIVATE ${PROJECT_SOURCE_DIR}/cppdiag)
target_link_libraries(test-cppdiag PRIVATE cppdiag PRIVATE cpputil PRIVATE Catch2::Catch2WithMain)

if (MSVC)
    target_compile_options(test-cppdiag PRIVATE "/W4")
else ()
    target_compile_options(test-cppdiag PRIVATE "-Wall" "-Wextra" "-Wpedantic")
endif ()

include(Catch)
catch_discover_tests(test-cppdiag)
