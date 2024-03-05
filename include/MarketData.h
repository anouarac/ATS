/**
 * @file MarketData.h
 * @author Anouar Achghaf
 * @date 12/02/2023
 * @brief Contains the declaration of the MarketData class, which handles the streaming of market data for the trading system.
*/

#ifndef ATS_MARKETDATA_H
#define ATS_MARKETDATA_H
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include "ExchangeManager.h"
#include "OrderManager.h"

namespace ats {
    /**
     * @brief The Klines stores kline data for a specific symbol.
     */
    struct Klines {
         std::vector<time_t> times;
         std::vector<double> opens;
         std::vector<double> highs;
         std::vector<double> lows;
         std::vector<double> closes;
         std::vector<double> volumes;

         Klines() = default;

         void push_back(time_t time, double open, double high, double low, double close, double volume) {
             times.push_back(time);
             opens.push_back(open);
             highs.push_back(high);
             lows.push_back(low);
             closes.push_back(close);
             volumes.push_back(volume);
         }

         void pop_back() {
             if (times.empty()) return;
             times.pop_back();
             opens.pop_back();
             highs.pop_back();
             lows.pop_back();
             closes.pop_back();
             volumes.pop_back();
         }
     };

    /**
     * @brief Handles the streaming of market data for the trading system.
     */
    class MarketData {
    private:
        std::thread mMarketDataThread; /**< The thread used to run the market data stream */
        std::mutex mDataMutex; /**< A mutex used to protect access to the market data */
        bool mRunning; /**< Flag indicating whether the market data stream is running */
        std::unordered_set<std::string> mSymbols; /**< The set of symbols to subscribe to for market data */
        std::unordered_map<std::string, std::vector<double>> mPrices; /**< The current prices for each subscribed symbol */
        ExchangeManager& mExchangeManager; /**< A reference to the exchange manager used to retrieve market data */
        time_t mUpdateInterval; /**< Interval between updates of locally recorded data */
        std::unordered_map<std::string,OrderBook> mOrderBooks; /**< The order books for each subscribed symbol */
        std::map<std::string,double> mBalances; /**< User balance for each symbol */
        std::map<std::pair<std::string,std::string>,Klines> mKlines; /**< Kline data for symbol,interval pairs */

    public:
        /**
         * @brief Constructs a new MarketData object.
         * @param ems A reference to the ExchangeManager object used to retrieve market data.
         * @param updateInterval The interval between updates of local data, defaults to 1s.
         */
        MarketData(ExchangeManager& ems, time_t updateInterval=1);

        /**
         * @brief Destroys the MarketData object.
         */
        ~MarketData();

        /**
         * @brief Constructs a new MarketData object.
         * @param symbols A vector of symbols to subscribe to for market data.
         * @param ems A reference to the ExchangeManager object used to retrieve market data.
         * @param updateInterval The interval between updates of local data, defaults to 1s.
         */
        explicit MarketData(const std::vector<std::string>& symbols, ExchangeManager& ems, time_t updateInterval=1);

        /**
         * @brief Starts the market data stream.
         */
        void start();

        /**
         * @brief Runs the market data stream.
         */
        void run();

        /**
         * @brief Stops the market data stream.
         */
        void stop();

        /**
         * @brief Checks whether the market data stream is running.
         * @return True if the market data stream is running, false otherwise.
         */
        bool isRunning();

        /**
         * @brief Subscribes to a symbol for market data.
         * @param symbol The symbol to subscribe to.
         * @param interval The interval for the Kline we want to subscribe to
         */
        void subscribe(const std::string& symbol, std::string interval="3m");

        /**
         * @brief Unsubscribes from a symbol for market data.
         * @param symbol The symbol to unsubscribe from.
         */
        void unsubscribe(const std::string& symbol);

        /**
         * @brief Retrieves the current price for a symbol.
         * @param symbol The symbol to retrieve the price for.
         * @return The current price for the symbol.
         */
        double getPrice(const std::string& symbol);

        /**
         * @brief Returns prices recorded for a symbol.
         * @param symbol The symbol to retrieve the prices for.
         * @return The vector of prices recorded.
         */
         std::vector<double> getPrices(const std::string& symbol);

         /**
          * @brief Returns trade history for a symbol.
          * @param symbol The symbol to retrieve the history for.
          * @return The vector of trades.
          */
          std::vector<Trade> getTradeHistory(const std::string& symbol);

        /**
         * @brief Retrieves the quantity for a given price and symbol.
         * @param symbol The symbol to retrieve the quantity for.
         * @param price The price to retrieve the quantity for.
         * @return The quantity for the given price and symbol.
         */
        double getQtyForPrice(const std::string& symbol, double price);

        /**
         * @brief Retrives user's balances.
         * @return Map of pairs {asset, balance}.
         */
         std::map<std::string,double> getBalances();

         /**
          * @brief Retrieves order status.
          *
          * @param Id Client-side order Id.
          *
          * @return Order status
          */
          std::string getOrderStatus(long id, const std::string& symbol);

         /**
          * @brief Retrieves the OrderBook.
          * @param symbol Symbol for which to get the order book.
          * @return Current OrderBook.
          */
          OrderBook getOrderBook(const std::string &symbol);

          /**
           * @brief Retrieves Kline data.
           * @param symbol Symbol for which to get the Kline data.
           * @param interval Interval for the Klines.
           * @return The requested Kline data.
           */
           Klines getKlines(const std::string &symbol, const std::string& interval);

    private:
        /**
         * @brief Updates the price for a symbol.
         * @param symbol The symbol to update the price for.
         */
        void updatePrice(const std::string& symbol);

        /**
         * @brief Updates the prices for all subscribed symbols.
         */
        void updatePrices();

        /**
         * @brief Updates the Klines data for all symbols.
         */
         void updateKlines();

        /**
         * @brief Updates the order book for a symbol.
         * @param symbol The symbol to update the price for.
         */
        void updateOrderBook(const std::string& symbol);

        /**
         * @brief Updates the order books for all subscribed symbols.
         */
        void updateOrderBooks();

        /**
         * @brief Updates balances.
         */
        void updateBalances();

        /**
         * @brief Converts Json object to double.
         */
         double jsonToDouble(Json::Value res);
    };


} // ats

#endif //ATS_MARKETDATA_H
