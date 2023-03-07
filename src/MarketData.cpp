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

    double MarketData::getPrice(const std::string symbol) {
        std::unique_lock<std::mutex> lock(mDataMutex);
        if (mPrices.count(symbol)) {
            if (mPrices[symbol].empty()) {
                lock.unlock();
                updatePrice(symbol);
            }
            return mPrices[symbol].back();
        }
        return -1;
    }

    std::vector<double> MarketData::getPrices(std::string symbol) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        return mPrices[symbol];
    }

    std::vector<Trade> MarketData::getTradeHistory(std::string symbol) {
        return mExchangeManager.getTradeHistory(symbol);
    }

    void MarketData::updatePrice(const std::string& symbol) {
        std::unique_lock<std::mutex> lock(mDataMutex);
        if (mPrices[symbol].size() == 1000)
            mPrices[symbol].erase(mPrices[symbol].begin());
        lock.unlock();
        double price = mExchangeManager.getPrice(symbol);
        lock.lock();
        mPrices[symbol].push_back(price);
    }

    void MarketData::updatePrices() {
        std::unique_lock<std::mutex> lock(mDataMutex);
        auto mSymbolsCopy = mSymbols;
        lock.unlock();
        for (const std::string& symbol : mSymbolsCopy)
            updatePrice(symbol);
    }

    bool MarketData::isRunning() {
        return mRunning;
    }

    double MarketData::getQtyForPrice(const std::string &symbol, double price) {
        return price / getPrice(symbol);
    }

    std::map<std::string,double> MarketData::getBalances() {
        return mExchangeManager.getBalances();
    }

} // ats