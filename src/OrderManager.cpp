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

    OrderType stringToOrderType(const std::string &s) {
        for (int i = 0; i < OTCOUNT; i++)
            if (OrderTypeToString(OrderType(i)) == s)
                return OrderType(i);
        return OTCOUNT;
    }

    std::string SideToString(Side s) {
        switch (s) {
            case BUY:
                return "BUY";
            case SELL:
                return "SELL";
            default:
                return "Unknown";
        }
    }

    Side stringToSide(const std::string &s) {
        for (int i = 0; i < SCOUNT; i++)
            if (SideToString(Side(i)) == s)
                return Side(i);
        return SCOUNT;
    }

    OrderManager::OrderManager() {
        start();
    }

    OrderManager::OrderManager(std::vector<std::string> symbols) {
        for (std::string symbol : symbols)
            mSymbols.insert(symbol);
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
        mSymbols.insert(symbol);
        mPendingOrders.push(Order(id, type, side, symbol, quantity, price));
    }

    void OrderManager::createOrder(Order order) {
        mSymbols.insert(order.symbol);
        order.id = getNewOrderId();
        mPendingOrders.push(order);
    }

    void OrderManager::cancelOrder(long orderId, std::string symbol) {
        mCancelOrders.push({orderId, symbol});
    }

    void OrderManager::cancelAllOrders() {
        std::lock_guard<std::mutex> lock(mOrderFetchMutex);
        for (auto &pair: mSentOrders)
            cancelOrder(pair.second.id, pair.second.symbol);
    }

    void OrderManager::updateOpenOrders(std::unordered_map<long, Order> openOrders) {
        std::lock_guard<std::mutex> lock(mOrderFetchMutex);
        mSentOrders.swap(openOrders);
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
        std::lock_guard<std::mutex> lock(mOrderCountMutex);
        return mOrderCount++;
    }

    bool OrderManager::hasOrders() {
        return !mOrders.empty();
    }

    bool OrderManager::hasCancelOrders() {
        return !mCancelOrders.empty();
    }

    Order &OrderManager::getOldestOrder() {
        Order oldest = mOrders.front();
        mOrders.pop();
        std::lock_guard<std::mutex> lock(mOrderFetchMutex);
        mSentOrders.insert({oldest.id, oldest});
        return mSentOrders.find(oldest.id)->second;
    }

    std::pair<long, std::string> OrderManager::getCancelOrder() {
        std::pair<long, std::string> order = mCancelOrders.front();
        mCancelOrders.pop();
        return order;
    }

    std::vector<std::string> OrderManager::getSymbols() {
        std::vector<std::string> vSymbols;
        for (const std::string& symbol : mSymbols)
            vSymbols.push_back(symbol);
        return vSymbols;
    }
} // ats