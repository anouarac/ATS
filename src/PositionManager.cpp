//
// Created by Anouar Achghaf on 15/02/2023.
//

#include "PositionManager.h"

namespace ats {

    PositionManager::PositionManager(MarketData &marketData) : mData(marketData) {
        mPnL = 0;
        mRunning = false;
        start();
    }

    PositionManager::~PositionManager() {
        stop();
    }

    void PositionManager::start() {
        mRunning = true;
        mPositionManagerThread = std::thread(&PositionManager::run, this);
    }

    void PositionManager::run() {
        while (mRunning)
            updatePnL();
    }

    void PositionManager::stop() {
        mRunning = false;
        if (mPositionManagerThread.joinable())
            mPositionManagerThread.join();
    }

    bool PositionManager::isRunning() {
        return mRunning;
    }

    double PositionManager::getPnL() {
        return mPnL;
    }

    double PositionManager::getPosition(std::string symbol) {
        if (mOpenPositions.count(symbol))
            return mOpenPositions[symbol].quantity;
        return 0;
    }

    void PositionManager::updatePosition(std::string symbol, double quantity) {
        std::unique_lock<std::mutex> lock(mPositionMutex);
        if (mOpenPositions.count(symbol))
            mOpenPositions[symbol].quantity += quantity;
        else mOpenPositions[symbol] = Position(quantity, mData.getPrice(symbol));
    }

    void PositionManager::updatePnL() {
        for (auto openPosition : mOpenPositions) {
            std::string symbol = openPosition.first;
            Position position = openPosition.second;
            std::unique_lock<std::mutex> lock(mPositionMutex);
            mPnL -= position.total();
            position.price = mData.getPrice(symbol);
            mPnL += position.total();
        }
    }
} // ats