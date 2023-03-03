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

namespace ats {
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

    public:
        /**
         * @brief Constructs a new MarketData object.
         * @param ems A reference to the ExchangeManager object used to retrieve market data.
         */
        MarketData(ExchangeManager& ems);

        /**
         * @brief Destroys the MarketData object.
         */
        ~MarketData();

        /**
         * @brief Constructs a new MarketData object.
         * @param symbols A vector of symbols to subscribe to for market data.
         * @param ems A reference to the ExchangeManager object used to retrieve market data.
         */
        explicit MarketData(const std::vector<std::string>& symbols, ExchangeManager& ems);

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
         */
        void subscribe(const std::string& symbol);

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
         * @brief Retrieves the quantity for a given price and symbol.
         * @param symbol The symbol to retrieve the quantity for.
         * @param price The price to retrieve the quantity for.
         * @return The quantity for the given price and symbol.
         */
        double getQtyForPrice(const std::string& symbol, double price);

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

    };


} // ats

#endif //ATS_MARKETDATA_H
