//
// Created by Anouar Achghaf on 16/02/2023.
//

#include "OrderManager.h"

namespace ats {
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

    void OrderManager::createOrder(std::string symbol, int quantity) {
        int id = getNewOrderId();
        mOrders.push(Order(id, symbol, quantity));
    }

    void OrderManager::processOrder(Order order) {
        // TODO: Add EMS
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