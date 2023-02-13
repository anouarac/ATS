cmake_minimum_required(VERSION 3.23)
project(ats)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE headers include/*.h)
file(GLOB_RECURSE sources src/*.cpp)

add_executable(${PROJECT_NAME} ${headers} ${sources} main.cpp)