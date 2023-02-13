//
// Created by Anouar Achghaf on 12/02/2023.
//

#ifndef ALGO_TRADING_STRATEGY_H
#define ALGO_TRADING_STRATEGY_H
#include <thread>
#include <vector>
#include "MarketData.h"

namespace ats {

    class Strategy {
    protected:
        MarketData &mData;
        std::string mSymbol;
        std::vector<double> mPrices;
        std::thread mStrategyThread;
        bool mRunning;
    public:
        Strategy(std::string symbol, MarketData &data, std::vector<double> prices={});
        virtual void start();
        virtual void run();
        virtual void stop();
    protected:
        virtual void updatePrice() = 0;
        virtual bool getSignal() = 0;
        virtual void buy();
        virtual void sell();
    };

} // ats

#endif //ALGO_TRADING_STRATEGY_H
