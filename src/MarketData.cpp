//
// Created by Anouar Achghaf on 12/02/2023.
//

#include "../include/MarketData.h"
#include <vector>

namespace ats {

    MarketData::MarketData(ExchangeManager& ems) : mExchangeManager(ems) {}

    MarketData::MarketData(const std::vector<std::string>& symbols, ExchangeManager& ems) : mExchangeManager(ems) {
        for (const std::string& symbol : symbols)
            subscribe(symbol);
        mRunning = false;
        start();
    }

    MarketData::~MarketData() {
        MarketData::stop();
    }
    void MarketData::start() {
        mRunning = true;
        mMarketDataThread = std::thread(&MarketData::run, this);
    }

    void MarketData::run() {
        while (mRunning)
            updatePrices();
    }

    void MarketData::stop() {
        mRunning = false;
        if (mMarketDataThread.joinable())
            mMarketDataThread.join();
    }

    void MarketData::subscribe(const std::string& symbol) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        mSymbols.insert(symbol);
        mPrices[symbol] = {};
    }

    void MarketData::unsubscribe(const std::string& symbol) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        mSymbols.erase(symbol);
        mPrices.erase(symbol);
    }

    double MarketData::getPrice(const std::string& symbol) {
        if (mPrices.count(symbol)) {
            updatePrice(symbol);
            return (mPrices[symbol].empty() ? 0 : mPrices[symbol].back());
        }
        return -1;
    }

    void MarketData::updatePrice(const std::string& symbol) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        if (mPrices[symbol].size() == 10)
            mPrices[symbol].erase(mPrices[symbol].begin());
        mPrices[symbol].push_back(mExchangeManager.getPrice(symbol));
    }

    void MarketData::updatePrices() {
        for (const std::string& symbol : mSymbols)
            updatePrice(symbol);
    }

    bool MarketData::isRunning() {
        return mRunning;
    }

    double MarketData::getQtyForPrice(const std::string &symbol, double price) {
        return price / getPrice(symbol);
    }

} // ats