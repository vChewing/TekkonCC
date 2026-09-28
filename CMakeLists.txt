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
# 測項檔案清單。注意：`./GTests/main.cc` **刻意不收**——本靶連結 `gtest_main`（見下行），
# main 由該處提供；再收 main.cc 即為重複定義。
add_executable(TekkonTest
        ./GTests/TekkonTest.cc
        ./GTests/TekkonTests_Arrangements.cc
        ./GTests/TekkonTests_Performance.cc
        ./GTests/TekkonTests_PhonabetAutoChopPredicate.cc
        ./GTests/TekkonTests_Pinyin.cc
        ./GTests/TekkonTests_SequentialValidation.cc
        ./GTests/TekkonTests_SyllableIndex.cc)
target_link_libraries(TekkonTest gtest_main TekkonLib)
include(GoogleTest)
gtest_discover_tests(TekkonTest)
# `gtest_discover_tests` 只是寫出各測項之 add_test 指令；若不開 enable_testing()，
# 本目錄即不生成 CTestTestfile.cmake，`ctest` 會回報 0 個測項（而 runTest 仍可跑）。
enable_testing()

add_custom_target(
        runTest
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/TekkonTest
)
add_dependencies(runTest TekkonTest)
