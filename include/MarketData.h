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
        explicit MarketData(std::vector<std::string> symbols);
        void start();
        void run();
        void stop();
        void subscribe(std::string symbol);
        void unsubscribe(std::string symbol);
        double getPrice(std::string symbol);
    private:
        void updatePrice(std::string symbol);
        void updatePrices();
    };

} // ats

#endif //ALGO_TRADING_MARKETDATA_H
