//
// Created by Anouar Achghaf on 16/02/2023.
//

#include "OrderManager.h"

namespace ats {

    std::string OrderTypeToString(OrderType t) {
        switch (t) {
            case LIMIT:
                return "LIMIT";
            case MARKET:
                return "MARKET";
            case STOP_LOSS:
                return "STOP_LOSS";
            case STOP_LOSS_LIMIT:
                return "STOP_LOSS_LIMIT";
            case TAKE_PROFIT:
                return "TAKE_PROFIT";
            case TAKE_PROFIT_LIMIT:
                return "TAKE_PROFIT_LIMIT";
            case LIMIT_MAKER:
                return "LIMIT_MAKER";
            default:
                return "Unknown type";
        }
    }

    std::string SideToString(Side s) {
        switch(s) {
            case BUY:
                return "BUY";
            case SELL:
                return "SELL";
            default:
                return "Unknown";
        }
    }

    OrderManager::OrderManager() {
        start();
    }

    OrderManager::~OrderManager() {
        stop();
    }

    void OrderManager::start() {
        mRunning = true;
        mOrderManagerThread = std::thread(&OrderManager::run, this);
    }

    void OrderManager::run() {
        processOrders();
    }

    void OrderManager::stop() {
        mRunning = false;
        if (mOrderManagerThread.joinable())
            mOrderManagerThread.join();
    }

    bool OrderManager::isRunning() {
        return mRunning;
    }

    void OrderManager::createOrder(OrderType type, Side side, std::string symbol, double quantity, double price) {
        int id = getNewOrderId();
        mPendingOrders.push(Order(id, MARKET, side, symbol, quantity, price));
    }

    void OrderManager::processOrder(Order order) {
        bool valid = true;
        if (valid)
            mOrders.push(order);
    }

    void OrderManager::processOrders() {
        while (mRunning)
            if (!mOrders.empty()) {
                std::unique_lock<std::mutex> lock(mOrderFetchMutex);
                Order order = mOrders.front();
                mOrders.pop();
                lock.unlock();
                processOrder(order);
            }
    }

    int OrderManager::getNewOrderId() {
        std::unique_lock<std::mutex> lock(mOrderCountMutex);
        return mOrderCount++;
    }
} // ats