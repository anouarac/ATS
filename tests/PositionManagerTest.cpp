//
// Created by Anouar Achghaf on 15/02/2023.
//
#include "PositionManager.h"
#include "MarketData.h"
#include "OrderManager.h"
#include "BinanceExchangeManager.h"
#include <gtest/gtest.h>

class PositionManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        exchangeManager = new ats::BinanceExchangeManager(orderManager);
        marketData = new ats::MarketData(*exchangeManager);
        marketData->start();
        marketData->subscribe("BTCUSDT");
        marketData->subscribe("ETHUSDT");
        positionManager = new ats::PositionManager(*marketData);
    }

    void TearDown() override {
        delete positionManager;
        delete marketData;
        delete exchangeManager;
    }

    ats::OrderManager orderManager;
    ats::BinanceExchangeManager* exchangeManager;
    ats::MarketData* marketData;
    ats::PositionManager* positionManager;
};

TEST_F(PositionManagerTest, TestStartAndStop) {
    ASSERT_TRUE(positionManager->isRunning());
    positionManager->stop();
    ASSERT_FALSE(positionManager->isRunning());
    positionManager->start();
}


TEST_F(PositionManagerTest, TestUpdatePosition) {
    positionManager->updatePosition("BTCUSDT", 10);
    ASSERT_DOUBLE_EQ(positionManager->getPosition("BTCUSDT"), 10);
    ASSERT_DOUBLE_EQ(positionManager->getPosition("ETHUSDT"), 0);
    ASSERT_DOUBLE_EQ(positionManager->getPosition("AAVEUSDT"), 0);
}

TEST_F(PositionManagerTest, TestGetPnL) {
    ASSERT_DOUBLE_EQ(positionManager->getPnL(), 0);
}
