//
// Created by Anouar Achghaf on 16/02/2023.
//
#include "OrderManager.h"
#include <gtest/gtest.h>

TEST(OrderManagerTest, CreateAndProcessOrder) {
    ats::OrderManager orderManager;
    orderManager.createOrder("AAPL", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for order to be processed
    // TODO: Add assertions to check that the order was processed correctly
}

TEST(OrderManagerTest, CreateAndProcessMultipleOrders) {
    ats::OrderManager orderManager;
    orderManager.createOrder("AAPL", 100);
    orderManager.createOrder("GOOG", 200);
    orderManager.createOrder("MSFT", 300);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Wait for orders to be processed
    // TODO: Add assertions to check that the orders were processed correctly in the correct order
}

// Test stopping and restarting the order manager
TEST(OrderManagerTest, StopAndRestart) {
    ats::OrderManager orderManager;
    orderManager.createOrder("AAPL", 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for order to be processed
    orderManager.stop();
    ASSERT_FALSE(orderManager.isRunning());
    orderManager.start();
    ASSERT_TRUE(orderManager.isRunning());
    orderManager.createOrder("GOOG", 200);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for order to be processed
    // TODO: Add assertions to check that the orders were processed correctly
}


