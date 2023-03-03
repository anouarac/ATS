//
// Created by Anouar Achghaf on 15/02/2023.
//
/**
 * @file PositionManager.h
 * @author Anouar Achghaf
 * @date 15/02/2023
 * @brief This header file contains the declaration of the PositionManager class, which is responsible for managing and tracking the open positions and PnL of a trading strategy.
 * The PositionManager class provides functionality for starting and stopping a separate thread for updating the open positions and PnL, as well as updating the position for a specific symbol.
*/

#ifndef ATS_POSITIONMANAGER_H
#define ATS_POSITIONMANAGER_H
#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include "MarketData.h"

namespace ats {

    /**
     * @brief Represents a position with its quantity and price.
     */
    struct Position{
        double quantity; ///< Quantity of the position
        double price; ///< Price of the position

        /**
         * @brief Default constructor that initializes the quantity and price to 0.
         */
        Position() : quantity(0), price(0) {}

        /**
         * @brief Constructor that initializes the quantity and price to the given values.
         * @param q Quantity of the asset
         * @param p Price of the asset
         */
        Position(double q, double p) : quantity(q), price(p) {}

        /**
         * @brief Calculates the total value of the position.
         * @return The total value of the position (quantity * price).
         */
        double total() {
            return quantity * price;
        }
    };

/**
 * @brief Manages the positions and the PnL of a trading system.
 */
    class PositionManager {
    private:
        MarketData &mData; ///< Market data used by the PositionManager
        std::thread mPositionManagerThread; ///< Thread used to run the PositionManager
        double mPnL; ///< Current PnL of the trading system
        std::unordered_map<std::string, Position> mOpenPositions; ///< Open positions managed by the PositionManager
        bool mRunning; ///< Flag indicating whether the PositionManager is running
        std::mutex mPositionMutex; ///< Mutex used to synchronize access to the open positions
    public:

        /**
         * @brief Default constructor that initializes the market data to a default instance.
         */
        PositionManager();

        /**
         * @brief Constructor that initializes the market data to the given instance.
         * @param marketData The market data to use.
         */
        PositionManager(MarketData &marketData);

        /**
         * @brief Destructor that stops the PositionManager if it is running.
         */
        ~PositionManager();

        /**
         * @brief Starts the PositionManager thread.
         */
        void start();

        /**
         * @brief Runs the PositionManager loop.
         */
        void run();

        /**
         * @brief Stops the PositionManager thread.
         */
        void stop();

        /**
         * @brief Returns whether the PositionManager is running.
         * @return True if the PositionManager is running, false otherwise.
         */
        bool isRunning();

        /**
         * @brief Returns the current PnL of the trading system.
         * @return The current PnL.
         */
        double getPnL();

        /**
         * @brief Returns the current position of the given symbol (quantity held).
         * @param symbol The symbol of the position to retrieve.
         * @return The current position of the given symbol.
         */
        double getPosition(std::string symbol);

        /**
         * @brief Updates the position of the given symbol.
         * @param symbol The symbol of the position to update.
         * @param quantity The new quantity of the position.
         */
        void updatePosition(std::string symbol, double quantity);

    private:
        /**
         * @brief Updates the PnL of the trading system.
         */
        void updatePnL();
    };


} // ats

#endif //ATS_POSITIONMANAGER_H
