cmake_minimum_required(VERSION 3.5)
project(ats)
project(AllTests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE headers include/*.h)
file(GLOB_RECURSE sources src/*.cpp)

# gtest

add_subdirectory(external/googletest)
include_directories(external/googletest/googletest/include)

# Project and library
add_executable(${PROJECT_NAME} ${headers} ${sources} main.cpp)
add_library(${PROJECT_NAME}_lib ${headers} ${sources})
target_include_directories(${PROJECT_NAME}_lib PUBLIC include)

# Testing
enable_testing()

# Build the test executable and link against the main library and gtest
file(GLOB_RECURSE TESTS tests/*.cpp)
add_executable(ats_test ${TESTS})
target_link_libraries(ats_test gtest_main ${PROJECT_NAME}_lib)

# Register the test executable with CTest
add_test(NAME ats_test COMMAND ats_test)