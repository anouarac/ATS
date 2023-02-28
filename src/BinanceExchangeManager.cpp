//
// Created by Anouar Achghaf on 24/02/2023.
//

#include "BinanceExchangeManager.h"
#include <iostream>

namespace ats {
    using namespace binance;
    BinanceExchangeManager::BinanceExchangeManager(OrderManager& orderManager, bool isSimulation, std::string api_key, std::string secret_key) :
    ExchangeManager(orderManager), mServer(isSimulation?Server("https://testnet.binance.vision",1):Server()),
    mMarket(mServer), mAccount(mServer, api_key, secret_key) {
        mMarket.getAndSaveExchangeInfo();
    }

    BinanceExchangeManager::~BinanceExchangeManager() {}

    void BinanceExchangeManager::sendOrder(Order &order) {
        Json::Value result;
        BINANCE_ERR_CHECK(mAccount.sendOrder(result, order.symbol.c_str(),
                        SideToString(order.side).c_str(), OrderTypeToString(order.type).c_str(),
                           "", order.quantity, order.price, "",
                           order.stopPrice, order.icebergQty, order.recvWindow));
        Logger::write_log(result.toStyledString().c_str());
    }

    void BinanceExchangeManager::modifyOrder(Order &order) {

    }

    void BinanceExchangeManager::cancelOrder(Order &order) {

    }

    void BinanceExchangeManager::getOrderStatus(Order &order, Json::Value &result) {

    }

    std::vector<Order> BinanceExchangeManager::getOpenOrders() {
        return std::vector<Order>();
    }

    std::vector<Order> BinanceExchangeManager::getOrderHistory() {
        return std::vector<Order>();
    }

    double BinanceExchangeManager::getPrice(std::string symbol) {
        double price = -1;
        BINANCE_ERR_CHECK(mMarket.getPrice(symbol.c_str(), price));
        return price;
    }


} // ats