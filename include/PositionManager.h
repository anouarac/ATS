//
// Created by Anouar Achghaf on 15/02/2023.
//

#ifndef ATS_POSITIONMANAGER_H
#define ATS_POSITIONMANAGER_H
#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include "MarketData.h"

namespace ats {

    struct Position{
        double quantity;
        double price;

        Position() : quantity(0), price(0) {}
        Position(double q, double p) : quantity(q), price(p) {}
        double total() {
            return quantity * price;
        }
    };

    class PositionManager {
    private:
        MarketData &mData;
        std::thread mPositionManagerThread;
        double mPnL;
        std::unordered_map<std::string, Position> mOpenPositions;
        bool mRunning;
        std::mutex mPositionMutex;
    public:
        PositionManager();
        PositionManager(MarketData &marketData);
        ~PositionManager();
        void start();
        void run();
        void stop();
        bool isRunning();
        double getPnL();
        double getPosition(std::string symbol);
        void updatePosition(std::string symbol, double quantity);
    private:
        void updatePnL();
    };

} // ats

#endif //ATS_POSITIONMANAGER_H
