//
// Created by Anouar Achghaf on 12/02/2023.
//

#include "../include/MarketData.h"
#include <vector>

namespace ats {

    MarketData::MarketData(ExchangeManager &ems, time_t interval) : mExchangeManager(ems), mUpdateInterval(interval) {}

    MarketData::MarketData(const std::vector<std::string> &symbols, ExchangeManager &ems, time_t interval)
            : mExchangeManager(ems), mUpdateInterval(interval) {
        for (const std::string &symbol: symbols)
            subscribe(symbol);
        mRunning = false;
        start();
    }

    MarketData::~MarketData() {
        MarketData::stop();
    }

    void MarketData::start() {
        if (!mRunning) {
            mRunning = true;
            mMarketDataThread = std::thread(&MarketData::run, this);
        }
    }

    void MarketData::run() {
        time_t lastUpd{0};
        while (mRunning) {
            time_t newUpd;
            time(&newUpd);
            if (difftime(newUpd, lastUpd) > mUpdateInterval) {
                updatePrices();
                updateKlines();
                updateOrderBooks();
                updateBalances();
                time(&lastUpd);
            }
        }
    }

    void MarketData::stop() {
        mRunning = false;
        if (mMarketDataThread.joinable())
            mMarketDataThread.join();
    }

    void MarketData::subscribe(const std::string &symbol, std::string interval) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        mSymbols.insert(symbol);
        mOrderBooks[symbol];
        mPrices[symbol];
        mKlines[{symbol, interval}];
        mOrderBooks[symbol];
    }

    void MarketData::unsubscribe(const std::string &symbol) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        mSymbols.erase(symbol);
        mPrices.erase(symbol);
        mOrderBooks.erase(symbol);
        std::vector<std::string> intervalsToErase;
        for (const auto &[key, kline] : mKlines) {
            if (key.first == symbol)
                intervalsToErase.push_back(key.second);
        }
        for (const std::string& interval : intervalsToErase)
            mKlines.erase({symbol, interval});
    }

    double MarketData::getPrice(const std::string& symbol) {
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

    std::vector<double> MarketData::getPrices(const std::string& symbol) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        return mPrices[symbol];
    }

    std::vector<Trade> MarketData::getTradeHistory(const std::string& symbol) {
        return mExchangeManager.getTradeHistory(symbol);
    }

    void MarketData::updatePrice(const std::string &symbol) {
        std::unique_lock<std::mutex> lock(mDataMutex);
        if (mPrices[symbol].size() == 1000)
            mPrices[symbol].erase(mPrices[symbol].begin(), mPrices[symbol].begin()+500);
        lock.unlock();
        double price = mExchangeManager.getPrice(symbol);
        lock.lock();
        mPrices[symbol].push_back(price);
    }

    void MarketData::updatePrices() {
        std::unique_lock<std::mutex> lock(mDataMutex);
        auto mSymbolsCopy = mSymbols;
        lock.unlock();
        for (const std::string &symbol: mSymbolsCopy)
            updatePrice(symbol);
    }

    void MarketData::updateKlines() {
        std::unique_lock<std::mutex> lock(mDataMutex);
        std::map<std::pair<std::string,std::string>, Klines> klinesCopy = mKlines;
        lock.unlock();
        for (auto &[key, klines] : klinesCopy) {
            Json::Value result;
            if (klines.times.size())
                mExchangeManager.getKlines(result, key.first, key.second, 0, 0, 10);
            else mExchangeManager.getKlines(result, key.first, key.second, 0, 0, 500);
            try {
                for (Json::Value::ArrayIndex i = 0; i < result.size(); i++) {
                    time_t t = jsonToDouble(result[i][0])/1000;
                    double o = jsonToDouble(result[i][1]);
                    double h = jsonToDouble(result[i][2]);
                    double l = jsonToDouble(result[i][3]);
                    double c = jsonToDouble(result[i][4]);
                    double v = jsonToDouble(result[i][5]);
                    if (!klines.times.empty() && t < klines.times.back()) continue;
                    if (!klines.times.empty() && t == klines.times.back())
                        klines.pop_back();
                    klines.push_back(t, o, h, l, c, v);
                }
            }
            catch (...) {
                return;
            }
        }
        lock.lock();
        mKlines.swap(klinesCopy);
    }

    bool MarketData::isRunning() {
        return mRunning;
    }

    double MarketData::getQtyForPrice(const std::string &symbol, double price) {
        return price / getPrice(symbol);
    }

    std::map<std::string, double> MarketData::getBalances() {
        std::lock_guard<std::mutex> lock(mDataMutex);
        return mBalances;
    }

    std::string MarketData::getOrderStatus(long id, const std::string& symbol) {
        Json::Value result;
        Order order{id};
        order.symbol = symbol;
        mExchangeManager.getOrderStatus(order, result);
        std::string status = result.get("status", "REJECTED").asString();
        return status;
    }

    OrderBook MarketData::getOrderBook(const std::string &symbol) {
        std::lock_guard<std::mutex> lock(mDataMutex);
        if (mOrderBooks.count(symbol))
            return mOrderBooks[symbol];
        return {};
    }


    Klines MarketData::getKlines(const std::string &symbol, const std::string& interval) {
        if (mKlines.count({symbol, interval}))
            return mKlines[{symbol, interval}];
        return {};
    }

    void MarketData::updateOrderBook(const std::string &symbol) {
        OrderBook orderBook = mExchangeManager.getOrderBook(symbol);
        std::lock_guard<std::mutex> lock(mDataMutex);
        mOrderBooks[symbol] = orderBook;
    }

    void MarketData::updateOrderBooks() {
        std::unique_lock<std::mutex> lock(mDataMutex);
        auto mSymbolsCopy = mSymbols;
        lock.unlock();
        for (const std::string &symbol: mSymbolsCopy)
            updateOrderBook(symbol);
    }

    void MarketData::updateBalances() {
        auto balances = mExchangeManager.getBalances();
        std::unique_lock<std::mutex> lock(mDataMutex);
        balances.swap(mBalances);
    }

    double MarketData::jsonToDouble(Json::Value res) {
        return atof(res.asString().c_str());
    }

} // ats