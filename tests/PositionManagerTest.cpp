//
// Created by Anouar Achghaf on 15/02/2023.
//
#include "PositionManager.h"
#include "MarketData.h"
#include <gtest/gtest.h>

class PositionManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        marketData.start();
        marketData.subscribe("BTC");
        marketData.subscribe("ETH");
        positionManager = new ats::PositionManager(marketData);
    }

    void TearDown() override {
        positionManager->stop();
        delete positionManager;
    }

    ats::MarketData marketData;
    ats::PositionManager* positionManager;
};

TEST_F(PositionManagerTest, TestStartAndStop) {
    ASSERT_TRUE(positionManager->isRunning());
    positionManager->stop();
    ASSERT_FALSE(positionManager->isRunning());
    positionManager->start();
}


TEST_F(PositionManagerTest, TestUpdatePosition) {
    positionManager->updatePosition("BTC", 10);
    ASSERT_DOUBLE_EQ(positionManager->getPosition("BTC"), 10);
    ASSERT_DOUBLE_EQ(positionManager->getPosition("ETH"), 0);
    ASSERT_DOUBLE_EQ(positionManager->getPosition("AAVE"), 0);
}

TEST_F(PositionManagerTest, TestGetPnL) {
    ASSERT_DOUBLE_EQ(positionManager->getPnL(), 0);
}
