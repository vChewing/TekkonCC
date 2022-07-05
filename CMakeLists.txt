cmake_minimum_required(VERSION 3.17)
project(Tekkon)

set(CMAKE_CXX_STANDARD 17)

add_library(TekkonLib Tekkon/Tekkon.hh Tekkon/main.cpp)

# Let CMake fetch Google Test for us.
# https://gitee.com/mirrors/googletest/tree/main/googletest#incorporating-into-an-existing-cmake-project
include(FetchContent)

FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://gitcode.net/mirrors/google/googletest/-/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5/googletest-609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test target declarations.
add_executable(TekkonTest TekkonCCTest_gtest/TekkonTest.cpp)
target_link_libraries(TekkonTest gtest_main TekkonLib)
include(GoogleTest)
gtest_discover_tests(TekkonTest)

add_custom_target(
        runTest
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/TekkonTest
)
add_dependencies(runTest TekkonTest)
