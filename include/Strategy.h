//
// Created by Anouar Achghaf on 12/02/2023.
//
/**
 * @file Strategy.h
 * @author Anouar Achghaf
 * @date 12/02/2023
 * @brief Defines an abstract interface for trading strategies
*/

#ifndef ATS_STRATEGY_H
#define ATS_STRATEGY_H
#include <thread>
#include <vector>
#include "MarketData.h"
#include "OrderManager.h"

namespace ats {

    /**
 * @brief Defines an abstract interface for trading strategies
 */
    class Strategy {
    protected:
        MarketData& mData; ///< MarketData object to get market information
        OrderManager& mOrderManager; ///< OrderManager object to create and manage orders
        std::string mSymbol; ///< The symbol the strategy is trading
        std::vector<double> mPrices; ///< Vector of historical prices
        std::thread mStrategyThread; ///< Thread for running the strategy
        bool mRunning; ///< Flag indicating if the strategy is running
    public:
        /**
         * @brief Constructs a Strategy object
         * @param symbol The symbol the strategy will trade
         * @param data MarketData object to get market information
         * @param orderManager OrderManager object to create and manage orders
         * @param prices Vector of historical prices (default empty)
         */
        Strategy(std::string symbol, MarketData& data, OrderManager& orderManager, std::vector<double> prices={});

        /**
         * @brief Destructs the Strategy object
         */
        virtual ~Strategy();

        /**
         * @brief Starts the strategy thread
         */
        virtual void start();

        /**
         * @brief Runs the strategy
         */
        virtual void run();

        /**
         * @brief Stops the strategy
         */
        virtual void stop();

        /**
         * @brief Returns whether the strategy is running
         * @return True if the strategy is running, false otherwise
         */
        bool isRunning();

    protected:
        /**
         * @brief Updates the historical price vector
         */
        virtual void updatePrice() = 0;

        /**
         * @brief Gets the trading signal
         * @return True if the strategy signals to buy, false if the strategy signals to sell
         */
        virtual double getSignal() = 0;

        /**
         * @brief Sends a buy order to the OrderManager
         */
        virtual void buy() = 0;

        /**
         * @brief Sends a sell order to the OrderManager
         */
        virtual void sell() = 0;
    };


} // ats

#endif //ATS_STRATEGY_H
