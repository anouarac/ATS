//
// Created by Anouar Achghaf on 24/02/2023.
//

#ifndef ATS_BINANCEEXCHANGEMANAGER_H
#define ATS_BINANCEEXCHANGEMANAGER_H
#include "ExchangeManager.h"
#include "binance.h"
#include "json/json.h"
#include "binance_logger.h"

namespace ats {
    using namespace binance;
    class BinanceExchangeManager : public ExchangeManager {
    private:
        Server mServer;
        Market mMarket;
        Account mAccount;
        bool isSimulation;

    public:
        explicit BinanceExchangeManager(OrderManager& orderManager, bool isSimulation=true, std::string api_key="", std::string secret_key="");
        ~BinanceExchangeManager();
        void sendOrder(Order &order) override;
        void modifyOrder(Order &order) override;
        void cancelOrder(Order &order) override;
        void getOrderStatus(Order &order, Json::Value &result) override;
        std::vector<Order> getOpenOrders() override;
        std::vector<Order> getOrderHistory() override;

        double getPrice(std::string symbol) override;
    };

} // ats

#endif //ATS_BINANCEEXCHANGEMANAGER_H
