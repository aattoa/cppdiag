add_executable(test-cppdiag internals.cpp interface.cpp)
target_link_libraries(test-cppdiag PRIVATE cppdiag PRIVATE cpputil)

if (MSVC)
    target_compile_options(test-cppdiag PRIVATE "/W4")
else ()
    target_compile_options(test-cppdiag PRIVATE "-Wall" "-Wextra" "-Wpedantic")
endif ()

include(FetchContent)
FetchContent_Declare(
    cppunittest
    GIT_REPOSITORY https://github.com/aattoa/cppunittest.git
    GIT_TAG        ccd07f2686df5c50461d564e09dc63490bf41870)
FetchContent_MakeAvailable(cppunittest)

include(cppunittest)
cppunittest_add_tests(test-cppdiag)
