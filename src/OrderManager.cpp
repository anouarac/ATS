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
                return "Unknown";
        }
    }

    OrderType stringToOrderType(const std::string& s) {
        for (int i = 0; i < OTCOUNT; i++)
            if (OrderTypeToString(OrderType(i)) == s)
                return OrderType(i);
        return OTCOUNT;
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

    Side stringToSide(const std::string& s) {
        for (int i = 0; i < SCOUNT; i++)
            if (SideToString(Side(i)) == s)
                return Side(i);
        return SCOUNT;
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
        long id = getNewOrderId();
        mPendingOrders.push(Order(id, MARKET, side, symbol, quantity, price));
    }

    void OrderManager::processOrder(Order order) {
        bool valid = true;
        if (valid)
            mOrders.push(order);
    }

    void OrderManager::processOrders() {
        while (mRunning)
            if (!mPendingOrders.empty()) {
                Order order = mPendingOrders.front();
                mPendingOrders.pop();
                processOrder(order);
            }
    }

    int OrderManager::getNewOrderId() {
        std::unique_lock<std::mutex> lock(mOrderCountMutex);
        return mOrderCount++;
    }

    bool OrderManager::hasOrders() {
        return !mOrders.empty();
    }

    Order& OrderManager::getOldestOrder() {
        Order oldest = mOrders.front();
        mOrders.pop();
        mSentOrders.push_back(oldest);
        return mSentOrders.back();
    }
} // ats