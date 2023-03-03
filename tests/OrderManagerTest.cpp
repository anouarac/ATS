//
// Created by Anouar Achghaf on 16/02/2023.
//
#include "OrderManager.h"
#include <gtest/gtest.h>

TEST(OrderManagerTest, CreateAndProcessOrder) {
    ats::OrderManager orderManager;
    orderManager.createOrder(ats::MARKET, ats::BUY, "BTCUSDT", 0, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for order to be processed
    // TODO: Add assertions to check that the order was processed correctly
}

TEST(OrderManagerTest, CreateAndProcessMultipleOrders) {
    ats::OrderManager orderManager;
    orderManager.createOrder(ats::MARKET, ats::BUY, "BTCUSDT", 0, 0);
    orderManager.createOrder(ats::MARKET, ats::BUY, "USDTBTC", 0, 0);
    orderManager.createOrder(ats::MARKET, ats::BUY, "BTCUSDT", 0, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Wait for orders to be processed
    // TODO: Add assertions to check that the orders were processed correctly in the correct order
}

// Test stopping and restarting the order manager
TEST(OrderManagerTest, StopAndRestart) {
    ats::OrderManager orderManager;
    orderManager.createOrder(ats::MARKET, ats::BUY, "BTCUSDT", 0, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for order to be processed
    orderManager.stop();
    ASSERT_FALSE(orderManager.isRunning());
    orderManager.start();
    ASSERT_TRUE(orderManager.isRunning());
    orderManager.createOrder(ats::MARKET, ats::BUY, "BTCUSDT", 0, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait for order to be processed
    // TODO: Add assertions to check that the orders were processed correctly
}


