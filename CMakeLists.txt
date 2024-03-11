cmake_minimum_required(VERSION 3.17)
project(Tekkon)

set(CMAKE_CXX_STANDARD 17)

add_library(TekkonLib ./Sources/Tekkon/include/Tekkon.hh ./Sources/Tekkon/Tekkon.cc)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
        cmake_policy(SET CMP0135 NEW)
endif()

# Let CMake fetch Google Test for us.
# https://gitee.com/mirrors/googletest/tree/main/googletest#incorporating-into-an-existing-cmake-project
include(FetchContent)

FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test target declarations.
add_executable(TekkonTest ./GTests/TekkonTest.cc)
target_link_libraries(TekkonTest gtest_main TekkonLib)
include(GoogleTest)
gtest_discover_tests(TekkonTest)

add_custom_target(
        runTest
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/TekkonTest
)
add_dependencies(runTest TekkonTest)
