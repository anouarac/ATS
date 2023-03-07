//
// Created by Anouar Achghaf on 24/02/2023.
//

#include "BinanceExchangeManager.h"
#include <iostream>

namespace ats {
    using namespace binance;

    BinanceExchangeManager::BinanceExchangeManager(OrderManager &orderManager, bool isSimulation, std::string api_key,
                                                   std::string secret_key) :
            ExchangeManager(orderManager),
            mServer(isSimulation ? Server("https://testnet.binance.vision", 1) : Server()),
            mMarket(mServer), mAccount(mServer, api_key, secret_key) {
        start();
    }

    BinanceExchangeManager::~BinanceExchangeManager() {
        stop();
    }

    void BinanceExchangeManager::start() {
        mRunning = true;
        updateOpenOrders();
        mExchangeManagerThread = std::thread(&BinanceExchangeManager::run, this);
    }

    void BinanceExchangeManager::run() {
        while (mRunning) {
            if (mOrderManager.hasOrders()) {
                Order &order = mOrderManager.getOldestOrder();
                sendOrder(order);
            }
            if (mOrderManager.hasCancelOrders()) {
                std::pair<long, std::string> order = mOrderManager.getCancelOrder();
                cancelOrder(order.first, order.second);
            }
            updateOpenOrders();
        }
    }

    void BinanceExchangeManager::stop() {
        mRunning = false;
        if (mExchangeManagerThread.joinable())
            mExchangeManagerThread.join();
    }

    bool BinanceExchangeManager::isRunning() {
        return mRunning;
    }

    bool BinanceExchangeManager::isSimulation() {
        return mIsSimulation;
    }

    void BinanceExchangeManager::updateOpenOrders() {
        auto symbols = mOrderManager.getSymbols();
        std::unordered_map<long, Order> openOrders;
        for (std::string symbol: symbols) {
            auto orders = getOpenOrders(symbol);
            for (Order &order: orders) {
                order.id = order.emsId;
                if (emsToOmsId.count(order.emsId)) {
                    order.id = emsToOmsId[order.emsId];
                    omsToEmsId[order.id] = order.emsId;
                } else {
                    emsToOmsId[order.emsId] = order.id;
                    omsToEmsId[order.id] = order.emsId;
                }
                openOrders.insert({order.id, order});
            }
        }
        mOrderManager.updateOpenOrders(openOrders);
    }

    void BinanceExchangeManager::sendOrder(Order &order) {
        Json::Value result;
        BINANCE_ERR_CHECK(mAccount.sendOrder(result, order.symbol.c_str(),
                                             SideToString(order.side).c_str(), OrderTypeToString(order.type).c_str(),
                                             order.timeInForce.c_str(), order.quantity, order.price, "",
                                             order.stopPrice, order.icebergQty, order.recvWindow));
        Logger::write_log(result.toStyledString().c_str());
        order.emsId = result["orderId"].asInt64();
        omsToEmsId[order.id] = order.emsId;
        emsToOmsId[order.emsId] = order.id;
    }

    void BinanceExchangeManager::modifyOrder(Order &oldOrder, Order &newOrder) {
        cancelOrder(oldOrder);
        sendOrder(newOrder);
    }

    void BinanceExchangeManager::cancelOrder(long id, std::string symbol) {
        Json::Value result;
        BINANCE_ERR_CHECK(
                mAccount.cancelOrder(result, symbol.c_str(), omsToEmsId[id], "", "", 0));
        Logger::write_log(result.toStyledString().c_str());
    }

    void BinanceExchangeManager::cancelOrder(Order &order) {
        cancelOrder(order.id, order.symbol);
    }

    void BinanceExchangeManager::getOrderStatus(Order &order, Json::Value &result) {
        BINANCE_ERR_CHECK(mAccount.getOrder(result, order.symbol.c_str(), omsToEmsId[order.id], "", order.recvWindow));
    }

    Order BinanceExchangeManager::jsonToOrder(Json::Value &result) {
        std::string symbol = result["symbol"].asString();
        long emsId = stol(result["orderId"].asString());
        long omsId = (emsToOmsId.find(emsId) != emsToOmsId.end() ? emsToOmsId[emsId] : 0);
        double price = stod(result["price"].asString());
        double quantity = stod(result["origQty"].asString());
        Side side = stringToSide(result["side"].asString());
        OrderType type = stringToOrderType(result["type"].asString());
        std::string timeInForce = result["timeInForce"].asString();
        double stopPrice = stod(result["stopPrice"].asString());
        double icebergQty = stod(result["icebergQty"].asString());
        long time = stol(result["time"].asString()) / 1000;

        return Order{omsId, type, side, symbol, quantity, price,
                     stopPrice, icebergQty, 0, emsId, timeInForce, time};
    }

    std::vector<Order> BinanceExchangeManager::getOpenOrders(std::string symbol) {
        Json::Value result;
        std::vector<Order> orders;
        if (!mAccount.keysAreSet()) {
            Logger::write_log("<getOpenOrders> Keys not set");
            return orders;
        }
        if (symbol != "")
            BINANCE_ERR_CHECK(mAccount.getOpenOrders(result, symbol.c_str()));
        else
            BINANCE_ERR_CHECK(mAccount.getOpenOrders(result));
        for (Json::Value::ArrayIndex i = 0; i < result.size(); i++)
            orders.push_back(jsonToOrder(result[i]));
        return orders;
    }

    Trade BinanceExchangeManager::jsonToTrade(Json::Value &result) {
        long id = stol(result["id"].asString());
        double price = stod(result["price"].asString());
        double quantity = stod(result["qty"].asString());
        double quoteQty = stod(result["quoteQty"].asString());
        long time = stol(result["time"].asString());
        bool isBuyerMaker = result["isBuyerMaker"].asString() == "true";
        bool isBestMatch = result["isBestMatch"].asString() == "true";
        return Trade{id, price, quantity, quoteQty, time, isBuyerMaker, isBestMatch};
    }

    std::vector<Trade> BinanceExchangeManager::getTradeHistory(std::string symbol) {
        if (!mAccount.keysAreSet()) {
            Logger::write_log("<getTradeHistory> Keys not set");
            return {};
        }
        Json::Value result;
        std::vector<Trade> trades;
        BINANCE_ERR_CHECK(mAccount.getHistoricalTrades(result, symbol.c_str()));
        for (Json::Value::ArrayIndex i = 0; i < result.size(); i++)
            trades.push_back(jsonToTrade(result[i]));
        return trades;
    }

    double BinanceExchangeManager::getPrice(std::string symbol) {
        double price = -1;
        BINANCE_ERR_CHECK(mMarket.getPrice(symbol.c_str(), price));
        return price;
    }


    void
    BinanceExchangeManager::getKlines(Json::Value &result, std::string symbol, std::string interval, time_t start_date,
                                      time_t end_date, int limit) {
        BINANCE_ERR_CHECK(mMarket.getKlines(result, symbol.c_str(), interval.c_str(), start_date, end_date, limit));
    }

    void BinanceExchangeManager::getUserInfo(Json::Value &result) {
        if (!mAccount.keysAreSet()) {
            Logger::write_log("<getUserInfo> Keys not set");
            return;
        }
        BINANCE_ERR_CHECK(mAccount.getInfo(result));
    }

    std::map<std::string, double> BinanceExchangeManager::getBalances() {
        Json::Value result;
        if (!mAccount.keysAreSet()) {
            Logger::write_log("<getBalances> Keys not set");
            return {};
        }
        getUserInfo(result);
        if (!result.isMember("balances")) return {};
        std::map<std::string, double> balances;
        for (Json::Value::ArrayIndex i = 0; i < result["balances"].size(); i++) {
            auto res = result["balances"][i];
            balances.insert(std::make_pair(res["asset"].asString(), stod(res["free"].asString())));
        }
        return balances;
    }

} // ats