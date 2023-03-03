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
         */
        virtual void sendOrder(Order &order) = 0;

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
         * @return A vector of open orders.
         */
        virtual std::vector<Order> getOpenOrders() = 0;

        /**
         * @brief Retrieves the trade history for a given symbol on the exchange.
         *
         * @param symbol The symbol to retrieve the trade history for.
         * @return A vector of Trade objects representing the trade history.
         */
        virtual std::vector<Trade> getTradeHistory(std::string symbol) = 0;

        /**
         * @brief Retrieves the current price for a given symbol on the exchange.
         *
         * @param symbol The symbol to retrieve the price for.
         * @return The current price of the symbol.
         */
        virtual double getPrice(std::string symbol) = 0;
    };

}
#endif //ATS_EXCHANGEMANAGER_H
