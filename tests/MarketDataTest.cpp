//
// Created by Anouar Achghaf on 15/02/2023.
//
#include <gtest/gtest.h>
#include "MarketData.h"

TEST(MarketDataTest, TestSubscribeAndUnsuscribe) {
    ats::MarketData marketData;
    marketData.subscribe("BTC");
    EXPECT_TRUE(marketData.getPrice("BTC") >= 0);
    EXPECT_EQ(marketData.getPrice("ETH"), -1);
    marketData.unsubscribe("BTC");
    EXPECT_EQ(marketData.getPrice("BTC"), -1);
}

TEST(MarketDataTest, TestStartAndStop) {
    ats::MarketData marketData;
    marketData.start();
    ASSERT_TRUE(marketData.isRunning());
    marketData.stop();
    ASSERT_FALSE(marketData.isRunning());
}

TEST(MarketDataTest, TestUpdatePrice) {
    ats::MarketData marketData;
    marketData.start();
    marketData.subscribe("BTC");
    ASSERT_TRUE(marketData.getPrice("BTC") > 0);
    marketData.stop();
}