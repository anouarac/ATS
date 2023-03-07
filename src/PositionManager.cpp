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
        else {
            lock.unlock();
            double price = mData.getPrice(symbol);
            lock.lock();
            mOpenPositions[symbol] = Position(quantity, price);
        }
    }

    void PositionManager::updatePnL() {
        for (auto openPosition : mOpenPositions) {
            std::string symbol = openPosition.first;
            Position position = openPosition.second;
            double newPrice = mData.getPrice(symbol);
            std::lock_guard<std::mutex> lock(mPositionMutex);
            mPnL -= position.total();
            position.price = newPrice;
            mPnL += position.total();
        }
    }
} // ats