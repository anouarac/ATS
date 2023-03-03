//
// Created by Anouar Achghaf on 15/02/2023.
//
#include <gtest/gtest.h>
#include "MarketData.h"
#include "BinanceExchangeManager.h"
#include "OrderManager.h"

using namespace ats;

class MarketDataTest : public ::testing::Test {
protected:
    MarketData* md;
    BinanceExchangeManager* ems;
    OrderManager oms;
    std::vector<std::string> symbols{"BTCUSDT", "BNBUSDT", "LTCUSDT"};
    void SetUp() override {
        ems = new BinanceExchangeManager(oms);
        md = new MarketData(symbols, *ems);
    }
    void TearDown() override {
        delete md;
        delete ems;
    }
};

TEST_F(MarketDataTest, TestSubscription) {
    ASSERT_TRUE(md->isRunning());
    ASSERT_DOUBLE_EQ(md->getPrice("ETHUSDT"), -1);
    md->subscribe("ETHUSDT");
    ASSERT_GT(md->getPrice("ETHUSDT"), 0);
    md->unsubscribe("ETHUSDT");
    ASSERT_DOUBLE_EQ(md->getPrice("ETHUSDT"), -1);
}

TEST_F(MarketDataTest, TestStartStop) {
    md->stop();
    ASSERT_FALSE(md->isRunning());
    md->start();
    ASSERT_TRUE(md->isRunning());
}

TEST_F(MarketDataTest, TestPriceUpdate) {
    ASSERT_GT(md->getPrice("BTCUSDT"), 0);
    ASSERT_GT(md->getPrice("BNBUSDT"), 0);
}

TEST_F(MarketDataTest, TestQtyForPrice) {
    double price = md->getPrice("BTCUSDT");
    ASSERT_GT(price, 0);
    double qty = md->getQtyForPrice("BTCUSDT", price);
    ASSERT_GT(qty, 0);
    ASSERT_NEAR(qty, 1, 1e-3);
}