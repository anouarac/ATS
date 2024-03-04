/**
 *
 * @file ExchangeManager.h
 * @author Anouar Achghaf
 * @date 24/02/2023
 * @brief Contains an abstract class ExchangeManager
 * The ExchangeManager class is an abstract class that defines the interface for managing orders on an exchange.
*/



#ifndef ATS_EXCHANGEMANAGER_H
#define ATS_EXCHANGEMANAGER_H

#include <thread>
#include <mutex>
#include <queue>
#include "OrderManager.h"
#include "Trade.h"
#include "json/json.h"

namespace ats {
    /**
     * @brief The ExchangeManager class is an abstract class that defines the interface for managing orders on an exchange.
     *
     */
    class ExchangeManager {
    protected:
        OrderManager &mOrderManager; /**< A reference to the OrderManager the EMS will be retrieving orders from */

    public:

        /**
         * @brief Constructor for ExchangeManager class.
         *
         * @param oms A reference to the OrderManager object.
         */
        ExchangeManager(OrderManager &oms);

        /**
         * @brief Virtual destructor for ExchangeManager class.
         */
        virtual ~ExchangeManager() = default;

        /**
         * @brief Sends an order to the exchange.
         *
         * @param order The order to be sent to the exchange.
         * @return Filled quantity.
         */
        virtual double sendOrder(Order &order) = 0;

        /**
         * @brief Modifies an existing order on the exchange.
         *
         * @param oldOrder The original order.
         * @param newOrder The modified order.
         */
        virtual void modifyOrder(Order &oldOrder, Order &newOrder) = 0;

        /**
         * @brief Cancels an order on the exchange.
         *
         * @param order The order to be cancelled.
         */
        virtual void cancelOrder(Order &order) = 0;

        /**
         * @brief Retrieves the status of an order on the exchange.
         *
         * @param order The order to check the status of.
         * @param result A Json::Value object to store the order status information.
         */
        virtual void getOrderStatus(Order &order, Json::Value &result) = 0;

        /**
         * @brief Retrieves a list of open orders on the exchange.
         *
         * @param symbol The symbol to get the orders for, all open orders if omitted.
         * @return A vector of open orders.
         */
        virtual std::vector<Order> getOpenOrders(std::string symbol) = 0;

        /**
         * @brief Retrieves the trade history for a given symbol on the exchange.
         *
         * @param symbol The symbol to retrieve the trade history for.
         * @return A vector of Trade objects representing the trade history.
         */
        virtual std::vector<Trade> getTradeHistory(std::string symbol) = 0;

        /**
         * @brief Retrieves user balances.
         *
         * @return A map of pairs {asset, balance}.
         */
         virtual std::map<std::string,double> getBalances() = 0;

        /**
         * @brief Gets Klines for a symbol
         *
         * @param result Json object to write the response to
         * @param symbol Symbol to get the Klines for
         * @param interval Kline interval
         * @param start_date Start date
         * @param end_date End date
         * @param limit Limit, should be less than or equal to 1000
         */
         virtual void getKlines(Json::Value& result, std::string symbol, std::string interval, time_t start_date, time_t end_date, int limit) = 0;

        /**
         * @brief Retrieves the current price for a given symbol on the exchange.
         *
         * @param symbol The symbol to retrieve the price for.
         * @return The current price of the symbol.
         */
        virtual double getPrice(std::string symbol) = 0;

        /**
         * @brief Retrieves the order book.
         *
         * @param symbol The symbol to retrieve the order book for.
         * @return The bid and ask vectors.
         */
        virtual OrderBook getOrderBook(std::string symbol) = 0;
    };

}
#endif //ATS_EXCHANGEMANAGER_H
