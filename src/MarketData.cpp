//
// Created by Anouar Achghaf on 12/02/2023.
//

#include "../include/MarketData.h"
#include <vector>

namespace ats {

    MarketData::MarketData() = default;

    MarketData::MarketData(const std::vector<std::string>& symbols) {
        for (std::string symbol : symbols)
            mSymbols.insert(symbol);
        mRunning = false;
        MarketData::start();
    }

    MarketData::~MarketData() {
        MarketData::stop();
    }
    void MarketData::start() {
        mRunning = true;
        mMarketDataThread = std::thread(&MarketData::run, this);
    }

    void MarketData::run() {
        while (mRunning) {
            updatePrices();
        }
    }

    void MarketData::stop() {
        mRunning = false;
        if (mMarketDataThread.joinable())
            mMarketDataThread.join();
    }

    void MarketData::subscribe(const std::string& symbol) {
        mSymbols.insert(symbol);
        mPrices[symbol] = {};
    }

    void MarketData::unsubscribe(const std::string& symbol) {
        mSymbols.erase(symbol);
        mPrices.erase(symbol);
    }

    double MarketData::getPrice(const std::string& symbol) {
        if (mPrices.count(symbol))
            return (mPrices[symbol].empty()? 0 : mPrices[symbol].back());
        return -1;
    }

    void MarketData::updatePrice(const std::string& symbol) {
        std::unique_lock<std::mutex> lock(mDataMutex);
        if (mPrices[symbol].size() < 10)
            mPrices[symbol].push_back(5); // TODO: add EMS
    }

    void MarketData::updatePrices() {
        for (const std::string& symbol : mSymbols)
            updatePrice(symbol);
    }

    bool MarketData::isRunning() {
        return mRunning;
    }

} // ats