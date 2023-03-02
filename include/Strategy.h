//
// Created by Anouar Achghaf on 12/02/2023.
//

#ifndef ATS_STRATEGY_H
#define ATS_STRATEGY_H
#include <thread>
#include <vector>
#include "MarketData.h"

namespace ats {

    class Strategy {
    protected:
        MarketData& mData;
        ExchangeManager& mExchangeManager;
        std::string mSymbol;
        std::vector<double> mPrices;
        std::thread mStrategyThread;
        bool mRunning;
    public:
        Strategy(std::string symbol, MarketData& data, ExchangeManager& exchangeManager, std::vector<double> prices={});
        ~Strategy();
        virtual void start();
        virtual void run();
        virtual void stop();
        bool isRunning();
    protected:
        virtual void updatePrice() = 0;
        virtual bool getSignal() = 0;
        virtual void buy();
        virtual void sell();
    };

} // ats

#endif //ATS_STRATEGY_H
