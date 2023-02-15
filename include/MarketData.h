//
// Created by Anouar Achghaf on 12/02/2023.
//

#ifndef ALGO_TRADING_MARKETDATA_H
#define ALGO_TRADING_MARKETDATA_H
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace ats {

    class MarketData {
    private:
        std::thread mMarketDataThread;
        std::mutex mDataMutex;
        bool mRunning;
        std::unordered_set<std::string> mSymbols;
        std::unordered_map<std::string, std::vector<double>> mPrices;

    public:
        MarketData();
        ~MarketData();
        explicit MarketData(const std::vector<std::string>& symbols);
        void start();
        void run();
        void stop();
        bool isRunning();
        void subscribe(const std::string& symbol);
        void unsubscribe(const std::string& symbol);
        double getPrice(const std::string& symbol);
    private:
        void updatePrice(const std::string& symbol);
        void updatePrices();
    };

} // ats

#endif //ALGO_TRADING_MARKETDATA_H
