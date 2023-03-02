//
// Created by Anouar Achghaf on 24/02/2023.
//

#ifndef ATS_BINANCEEXCHANGEMANAGER_H
#define ATS_BINANCEEXCHANGEMANAGER_H
#include "ExchangeManager.h"
#include "thread"
#include "binance.h"
#include "json/json.h"
#include "binance_logger.h"

namespace ats {
    using namespace binance;

    struct BinanceTrade{

    };

    class BinanceExchangeManager : public ExchangeManager {
    private:
        Server mServer;
        Market mMarket;
        Account mAccount;
        bool mIsSimulation;
        bool mRunning;
        std::thread mExchangeManagerThread;
        std::map<long, long> omsToEmsId, emsToOmsId;

    public:
        explicit BinanceExchangeManager(OrderManager& orderManager, bool isSimulation=true, std::string api_key="", std::string secret_key="");
        ~BinanceExchangeManager();

        void start();
        void run();
        void stop();
        bool isRunning();

        void sendOrder(Order& order) override;
        void modifyOrder(Order& oldOrder, Order& newOrder) override;
        void cancelOrder(Order& order) override;
        void getOrderStatus(Order& order, Json::Value& result) override;
        std::vector<Order> getOpenOrders() override;
        std::vector<Trade> getTradeHistory(std::string symbol) override;

        double getPrice(std::string symbol) override;

        Order jsonToOrder(Json::Value &result);
        Trade jsonToTrade(Json::Value &result);
    };

} // ats

#endif //ATS_BINANCEEXCHANGEMANAGER_H
