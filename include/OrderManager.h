/**
 * @file OrderManager.h
 * @author Anouar Achghaf
 * @date 16/02/2023
 * @brief Contains the declaration of the OrderManager class and related enums and structs
*/

#ifndef ATS_ORDERMANAGER_H
#define ATS_ORDERMANAGER_H

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <unordered_map>
#include <set>

namespace ats {
    /**
      * @enum OrderType
      * @brief Enum for different types of orders.
      */
    enum OrderType {
        LIMIT,                  /**< Limit order */
        MARKET,                 /**< Market order */
        STOP_LOSS,              /**< Stop loss order */
        STOP_LOSS_LIMIT,        /**< Stop loss limit order */
        TAKE_PROFIT,            /**< Take profit order */
        TAKE_PROFIT_LIMIT,      /**< Take profit limit order */
        LIMIT_MAKER,            /**< Limit maker order */
        OTCOUNT                 /**< Number of order types */
    };

    /**
     * @brief Converts OrderType enum value to string.
     * @param t The OrderType enum value to convert.
     * @return A string representation of the OrderType value.
     */
    std::string OrderTypeToString(OrderType t);

    /**
     * @brief Converts a string to an OrderType enum value.
     * @param s The string to convert.
     * @return The corresponding OrderType enum value.
     */
    OrderType stringToOrderType(const std::string &s);

    /**
     * @enum Side
     * @brief Enum for buy/sell side of an order.
     */
    enum Side {
        BUY,        /**< Buy side of an order */
        SELL,       /**< Sell side of an order */
        SCOUNT      /**< Number of sides */
    };

    /**
     * @brief Converts Side enum value to string.
     * @param t The Side enum value to convert.
     * @return A string representation of the Side value.
     */
    std::string SideToString(Side t);

    /**
     * @brief Converts a string to a Side enum value.
     * @param s The string to convert.
     * @return The corresponding Side enum value.
     */
    Side stringToSide(const std::string &s);


    /**
     * @brief The Order struct represents an order to be placed on an exchange.
     */
    struct Order {

        long id; /**< The ID of the order. */
        std::string symbol; /**< The trading symbol of the order. */
        double quantity; /**< The quantity of the asset to buy/sell. */
        double price; /**< The price of the asset in the quote currency. */
        OrderType type; /**< The type of the order (e.g. LIMIT, MARKET, etc.). */
        Side side; /**< The side of the order (e.g. BUY or SELL). */
        double stopPrice; /**< The stop price of the order (only for STOP_LOSS, STOP_LOSS_LIMIT, TAKE_PROFIT, and TAKE_PROFIT_LIMIT orders). */
        double icebergQty; /**< The iceberg quantity of the order (only for LIMIT_MAKER orders). */
        long recvWindow; /**< The receive window of the order (in milliseconds). */
        long emsId; /**< The EMS ID of the order. */
        std::string timeInForce; /**< The time in force of the order (e.g. GTC, IOC, FOK, etc.). */
        time_t time; /**< The time when the order was sent. */

        Order(long id=-1) : id(id) {}

        /**
         * @brief The Order constructor.
         * @param id The order ID.
         * @param symbol The trading symbol of the order.
         * @param quantity The quantity of the asset to be traded in the order.
         * @param price The price per unit of the asset in the order.
         * @param type The type of the order (LIMIT, MARKET, STOP_LOSS, etc.).
         * @param side The side of the order (BUY or SELL).
         * @param stopPrice The stop price of the order (if applicable).
         * @param icebergQty The iceberg quantity of the order (if applicable).
         * @param recvWindow The receive window of the order (if applicable).
         * @param emsId The ID assigned by the EMS to the order (if applicable).
         * @param timeInForce The time in force of the order (if applicable).
         * @param time The time when the order was sent
         */
        Order(long id, OrderType type, Side side, std::string symbol, double quantity, double price,
              double stopPrice = 0., double icebergQty = 0., long recvWindow = 0, long emsId = 0,
              std::string timeInForce = "", time_t time=0) {
            this->id = id;
            this->side = side;
            this->symbol = symbol;
            this->quantity = quantity;
            this->type = type;
            this->price = price;
            this->stopPrice = stopPrice;
            this->icebergQty = icebergQty;
            this->recvWindow = recvWindow;
            this->emsId = emsId;
            this->timeInForce = timeInForce;
            this->time = time;
        }
    };

    /**
     * @brief The OrderBook struct represents the orderbook.
     */
    struct OrderBook{
        std::vector<double> bid; /**< The bid prices. */
        std::vector<double> bidVol; /**< The bid volumes. */
        std::vector<double> ask; /**< The ask prices. */
        std::vector<double> askVol; /**< The ask volumes. */
        /**
         * @brief The OrderBook constructor.
         * @param bid The bid prices.
         * @param bidVol The bid volumes.
         * @param ask The ask prices.
         * @param askVol The ask volumes.
         */
        OrderBook(std::vector<double> bid={}, std::vector<double> bidVol={}, std::vector<double> ask={}, std::vector<double> askVol={}) {
            this->bid = bid;
            this->bidVol = bidVol;
            this->ask = ask;
            this->askVol = askVol;
        }
    };

    /**
     * @brief A class for managing orders
     */
    class OrderManager {
    private:
        std::unordered_map<long,Order> mSentOrders; ///< A map of all orders sent
        std::queue<std::pair<long,std::string>> mCancelOrders; ///< A queue of orders waiting to be canceled (EMS side)
        std::queue<Order> mOrders; ///< A queue of orders waiting to be processed (EMS side)
        std::queue<Order> mPendingOrders; ///< A queue of orders that have been sent and are waiting for confirmation
        std::thread mOrderManagerThread; ///< A thread for processing orders
        std::mutex mOrderCountMutex; ///< A mutex for accessing the order count
        std::mutex mOrderFetchMutex; ///< A mutex for accessing mSentOrders
        bool mRunning; ///< A flag indicating if the order manager is running
        long mOrderCount; ///< A counter for the number of orders processed
        std::set<std::string> mSymbols; ///< A set of subscribed symbols
        double mLastOrderQty;
    public:
        /**
         * @brief Construct a new OrderManager object
         *
         */
        OrderManager();

        /**
         * @brief Construct a new OrderManager with an initial symbols list
         *
         * @param symbols an initial list of traded symbols
         */
         OrderManager(std::vector<std::string> symbols);

        /**
         * @brief Destroy the OrderManager object
         *
         */
        ~OrderManager();

        /**
         * @brief Start the order manager thread
         *
         */
        void start();

        /**
         * @brief Run the order manager loop
         *
         */
        void run();

        /**
         * @brief Stop the order manager thread
         *
         */
        void stop();

        /**
         * @brief Check if the order manager is running
         *
         * @return true if the order manager is running, false otherwise
         */
        bool isRunning();

        /**
         * @brief Get last filled order quantity
         *
         * @return -1 if not set, quantity of last filled order otherwise.
         */
         double getLastOrderQty();

         /**
          * @brief Sets last filled order quantity
          *
          */
          void setLastOrderQty(double qty);

        /**
         * @brief Create a new order and add it to the queue
         *
         * @param type The type of order
         * @param side The side of the order
         * @param symbol The symbol to trade
         * @param quantity The quantity to trade
         * @param price The price to trade
         *
         * @return Order Id
         */
        long createOrder(OrderType type, Side side, std::string symbol, double quantity, double price=0);

        /**
         * @brief Add an order to the queue
         *
         * @param order The order to add
         *
         * @return Order Id
         */
        long createOrder(Order& order);

        /**
         * @brief Cancel an order
         *
         * @param orderId ID of the order
         * @param symbol Symbol of the order
         *
         */
        void cancelOrder(long orderId, std::string symbol);

        /**
         * @brief Cancel all orders
         */
         void cancelAllOrders();

         /**
          * @brief Update open orders
          *
          * @param openOrders the new list of open orders
          */
          void updateOpenOrders(std::unordered_map<long,Order> openOrders);

        /**
         * @brief Process a single order
         *
         * @param order The order to process
         */
        void processOrder(Order order);

        /**
         * @brief Process all orders in the queue
         *
         */
        void processOrders();

        /**
         * @brief Check if there are orders to be sent
         *
         * @return true if there are orders waiting to be sent
         * @return false otherwise
         */
        bool hasOrders();

        /**
         * @brief Check if there are orders to be cancelled
         *
         * @return true if there are orders to cancel
         * @return false otherwise
         */
         bool hasCancelOrders();

        /**
         * @brief Get the oldest order from the order queue
         *
         * @return Order& The oldest order in the queue
         */
        Order &getOldestOrder();

        /**
         * @brief Returns order by ID
         *
         * @param ID ID of the order to return
         *
         * @return Order The requested order
         */
         Order getOrderById(long ID);

        /**
         * @brief Get an order to cancel
         *
         * @return {id,symbol} of the order to cancel
         */
         std::pair<long,std::string> getCancelOrder();

         /**
          * @brief Get symbols currently ordered
          *
          * @return std::vector<std::string> a vector of symbols
          */
          std::vector<std::string> getSymbols();

    private:
        /**
        * @brief Generate a new unique order ID
        *
        * @return int A new unique order ID
        */
        int getNewOrderId();
    };

} // ats

#endif //ATS_ORDERMANAGER_H
