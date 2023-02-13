//
// Created by Anouar Achghaf on 12/02/2023.
//

#include "../include/MarketData.h"
#include <vector>

namespace ats {

    MarketData::MarketData() = default;

    MarketData::MarketData(std::vector<std::string> symbols) {
        for (std::string symbol : symbols) {
            mSymbols.insert(symbol);
            mPrices[symbol] = {};
        }
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

    void MarketData::subscribe(std::string symbol) {
        mSymbols.insert(symbol);
    }

    void MarketData::unsubscribe(std::string symbol) {
        mSymbols.erase(symbol);
        mPrices.erase(symbol);
    }

    double MarketData::getPrice(std::string symbol) {
        std::unique_lock<std::mutex> lock(mDataMutex);
        return mPrices[symbol].back();
    }

    void MarketData::updatePrice(std::string symbol) {
        std::unique_lock<std::mutex> lock(mDataMutex);
        mPrices[symbol].push_back(5); // TODO: add EMS
    }

    void MarketData::updatePrices() {
        for (std::string symbol : mSymbols)
            updatePrice(symbol);
    }

} // ats