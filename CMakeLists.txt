cmake_minimum_required(VERSION 3.5)
project(ats)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE headers include/*.h)
file(GLOB_RECURSE sources src/*.cpp)

# Binance api

add_subdirectory(external/binance-cxx-api)
include_directories(external/binance-cxx-api/include)

# gtest

add_subdirectory(external/googletest)
include_directories(external/googletest/googletest/include)

# Build the project and the library
add_library(${PROJECT_NAME} ${headers} ${sources})
target_link_libraries(${PROJECT_NAME} binance-cxx-api)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Main executable
add_executable(main main.cpp)
target_link_libraries(main ${PROJECT_NAME})

# Testing
enable_testing()

# Build the test executable and link against the main library and gtest
file(GLOB_RECURSE TESTS tests/*.cpp)
add_executable(ats_test ${TESTS})
target_link_libraries(ats_test gtest_main binance-cxx-api ${PROJECT_NAME})

# Register the test executable with CTest
add_test(NAME ats_test COMMAND ats_test)