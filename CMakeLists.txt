cmake_minimum_required(VERSION 3.5)
project(ats)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE headers include/*.h)
file(GLOB_RECURSE sources src/*.cpp)

# Binance api

add_subdirectory(external/binance-cxx-api)
include_directories(external/binance-cxx-api/include)

# Exchange UI

add_subdirectory(external/exchange-ui/exchange-ui)
include_directories(external/exchange-ui/exchange-ui/common)

# gtest

add_subdirectory(external/googletest)
include_directories(external/googletest/googletest/include)

# Build the project and the library
add_library(${PROJECT_NAME} ${headers} ${sources})
target_link_libraries(${PROJECT_NAME} binance-cxx-api app)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Main executable
add_executable(main main.cpp)
target_link_libraries(main ${PROJECT_NAME})
target_compile_features(main PUBLIC cxx_std_17)

# Examples
## cancel_orders
add_executable(cancel_orders examples/cancel_orders.cpp)
target_link_libraries(cancel_orders ${PROJECT_NAME})
target_compile_features(cancel_orders PUBLIC cxx_std_17)
## exchange_ui
add_executable(exchange_ui examples/exchange_ui.cpp)
target_link_libraries(exchange_ui ${PROJECT_NAME})
target_compile_features(exchange_ui PUBLIC cxx_std_17)
## strategy
add_executable(strategy examples/strategy.cpp)
target_link_libraries(strategy ${PROJECT_NAME})
target_compile_features(strategy PUBLIC cxx_std_17)
## market_making
add_executable(market_making examples/market_making.cpp)
target_link_libraries(market_making ${PROJECT_NAME})
target_compile_features(market_making PUBLIC cxx_std_17)
## reset_testnet_balance
add_executable(reset_testnet_balance examples/reset_testnet_balance.cpp)
target_link_libraries(reset_testnet_balance ${PROJECT_NAME})
target_compile_features(reset_testnet_balance PUBLIC cxx_std_17)

# Testing
enable_testing()

# Build the test executable and link against the main library and gtest
file(GLOB_RECURSE TESTS tests/*.cpp)
add_executable(ats_test ${TESTS})
target_link_libraries(ats_test gtest_main binance-cxx-api ${PROJECT_NAME})

# Register the test executable with CTest
add_test(NAME ats_test COMMAND ats_test)