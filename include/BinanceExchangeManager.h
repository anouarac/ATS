/**

 * @file BinanceExchangeManager.h
 * @author Anouar Achghaf
 * @date 24/02/2023
 * @brief This class is responsible for managing orders and interacting with the Binance exchange API.
 * This class implements the ExchangeManager interface and provides functionality to send, modify, and
 * cancel orders, as well as get the status of open orders, get the trade history, and get the current
 * price of a symbol. It also provides a method to get the user's account information from the Binance API.
 * @note This class requires an active Binance API key and secret key to function properly, it is assumed
 * that the spot keys reside in $HOME/.binance/key and $HOME/.binance/secret
 * and that the spot testnet keys are in $HOME/.binance/test_key and $HOME/.binance/test_secret
*/

#ifndef ATS_BINANCEEXCHANGEMANAGER_H
#define ATS_BINANCEEXCHANGEMANAGER_H

#include "ExchangeManager.h"
#include "thread"
#include "binance.h"
#include "json/json.h"
#include "binance_logger.h"

namespace ats {
    using namespace binance;

    /**
     * @brief The BinanceExchangeManager class is a class that defines the interface for managing orders on the Binance exchange.
     *
     */
    class BinanceExchangeManager : public ExchangeManager {
    private:
        Server mServer; ///< Binance server object.
        Market mMarket; ///< Binance market object.
        Account mAccount; ///< Binance account object.
        bool mIsSimulation; ///< Flag to indicate if it is in simulation mode or not.
        bool mRunning; ///< Flag to indicate if the exchange manager thread is running or not.
        std::thread mExchangeManagerThread; ///< Thread for running the exchange manager.
        std::map<long, long> omsToEmsId, emsToOmsId; ///< Maps to track order IDs between OMS and EMS.

    public:
        /**
         * @brief Constructor for BinanceExchangeManager class.
         *
         * @param orderManager Reference to the OrderManager object.
         * @param isSimulation A boolean indicating whether the exchange is a simulation or not.
         * @param api_key The API key for the Binance exchange account.
         * @param secret_key The secret key for the Binance exchange account.
         */
        explicit BinanceExchangeManager(OrderManager &orderManager, bool isSimulation = true, std::string apiKey = "",
                                        std::string secretKey = "");

        /**
         * @brief Destructor for BinanceExchangeManager class.
         */
        ~BinanceExchangeManager();

        /**
         * @brief Start the BinanceExchangeManager thread.
         */
        void start();

        /**
         * @brief The BinanceExchangeManager processing function.
         */
        void run();

        /**
         * @brief Stop the BinanceExchangeManager thread.
         */
        void stop();

        /**
         * @brief Check if the BinanceExchangeManager thread is running.
         *
         * @return A boolean indicating whether the BinanceExchangeManager thread is running or not.
         */
        bool isRunning();

        /**
         * @brief Check if the EMS is connected to the test network.
         *
         * @return A boolean indicating whether the EMS is connected to the test network.
         */
        bool isSimulation();

        /**
         * @brief Update open orders on the local OMS
         *
         */
         void updateOpenOrders();

        /**
         * @brief Send an order to the Binance exchange.
         *
         * @param order The Order object to be sent.
         * @return Filled quantity.
         */
        double sendOrder(Order &order) override;

        /**
         * @brief Modify an existing order on the Binance exchange.
         *
         * @param oldOrder The old Order object to be modified.
         * @param newOrder The new Order object.
         */
        void modifyOrder(Order &oldOrder, Order &newOrder) override;

        /**
         * @brief Cancel an existing order on the Binance exchange.
         *
         * @param orderId The orderId of the order to cancel.
         * @param symbol The symbol of the order to cancel.
         */
         void cancelOrder(long orderId, std::string symbol);

        /**
         * @brief Cancel an existing order on the Binance exchange.
         *
         * @param order The Order to be cancelled.
         */
        void cancelOrder(Order &order) override;

        /**
         * @brief Gets the status of the given order.
         *
         * @param order   The order to check the status of.
         * @param result  The JSON object containing the result of the operation.
         */
        void getOrderStatus(Order &order, Json::Value &result) override;

        /**
         * @brief Gets all open orders for the current user.
         *
         * @param symbol The symbol to retrieve the orders for, all open orders if omitted.
         * @return A vector of all open orders.
         */
        std::vector<Order> getOpenOrders(std::string symbol="") override;

        /**
         * @brief Gets the trade history for the specified symbol.
         *
         * @param symbol The symbol to get the trade history for.
         * @return A vector of all trades for the specified symbol.
         */
        std::vector<Trade> getTradeHistory(std::string symbol) override;

        /**
         * @brief Gets the current price for the specified symbol.
         *
         * @param symbol The symbol to get the price for.
         * @return The current price for the specified symbol.
         */
        double getPrice(std::string symbol) override;

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
         void getKlines(Json::Value& result, std::string symbol, std::string interval, time_t start_date=0, time_t end_date=0, int limit=500);

        /**
         * @brief Converts a JSON object to an Order object.
         *
         * @param result The JSON object to convert.
         * @return An Order object created from the JSON object.
         */
        Order jsonToOrder(Json::Value &result);

        /**
         * @brief Converts a JSON object to a Trade object.
         *
         * @param result The JSON object to convert.
         * @return A Trade object created from the JSON object.
         */
        Trade jsonToTrade(Json::Value &result);

        /**
         * @brief Gets the current user's information.
         *
         * @param result The JSON object containing the result of the operation.
         */
        void getUserInfo(Json::Value &result);

        /**
         * @brief Gets user's balances.
         *
         * @returns Map of pairs {asset, Qty}.
         */
         std::map<std::string,double> getBalances() override;

        /**
        * @brief Retrieves the order book.
        *
        * @param symbol The symbol to retrieve the order book for.
        * @return The bid and ask vectors.
        */
        virtual OrderBook getOrderBook(std::string symbol) override;
    };

} // ats

#endif //ATS_BINANCEEXCHANGEMANAGER_H
