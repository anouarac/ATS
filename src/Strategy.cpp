//
// Created by Anouar Achghaf on 12/02/2023.
//

#include <iostream>
#include <utility>
#include "../include/Strategy.h"

namespace ats {
    Strategy::Strategy(std::string symbol, MarketData& data, ExchangeManager& exchangeManager, std::vector<double> prices)
    : mSymbol(std::move(symbol)), mData(data), mExchangeManager(exchangeManager), mPrices(std::move(prices)) {
        Strategy::start();
    }

    Strategy::~Strategy() {
        Strategy::stop();
    }

    void Strategy::start() {
        mRunning = true;
        mStrategyThread = std::thread(&Strategy::run, this);
    }

    void Strategy::stop() {
        mRunning = false;
        if (mStrategyThread.joinable())
            mStrategyThread.join();
    }

    void Strategy::run() {
        while (mRunning) {
            updatePrice();
            double signal = getSignal();
            if (signal > 0)
                sell();
            else if (signal < 0)
                buy();
        }
    }

    bool Strategy::isRunning() {
        return mRunning;
    }

    void Strategy::buy() {

        std::cout << "Buying " << mSymbol << std::endl;
    }

    void Strategy::sell() {

        std::cout << "Selling " << mSymbol << std::endl;
    }

} // ats