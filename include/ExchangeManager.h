//
// Created by Anouar Achghaf on 24/02/2023.
//

#ifndef ATS_EXCHANGEMANAGER_H
#define ATS_EXCHANGEMANAGER_H
#include <thread>
#include <mutex>
#include <queue>
#include "OrderManager.h"
#include "json/json.h"

namespace ats {
    class ExchangeManager {
    protected:
        OrderManager& mOrderManager;

    public:
        ExchangeManager(OrderManager& oms);
        virtual void sendOrder(Order &order) = 0;
        virtual void modifyOrder(Order &order) = 0;
        virtual void cancelOrder(Order &order) = 0;
        virtual void getOrderStatus(Order &order, Json::Value &result) = 0;
        virtual std::vector<Order> getOpenOrders() = 0;
        virtual std::vector<Order> getOrderHistory() = 0;

        virtual double getPrice(std::string symbol) = 0;
    };

}
#endif //ATS_EXCHANGEMANAGER_H
