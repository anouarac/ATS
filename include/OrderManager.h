//
// Created by Anouar Achghaf on 16/02/2023.
//

#ifndef ATS_ORDERMANAGER_H
#define ATS_ORDERMANAGER_H
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <map>
#include <vector>

namespace ats {
    enum OrderType {
        LIMIT,
        MARKET,
        STOP_LOSS,
        STOP_LOSS_LIMIT,
        TAKE_PROFIT,
        TAKE_PROFIT_LIMIT,
        LIMIT_MAKER,
        OTCOUNT
    };
    std::string OrderTypeToString(OrderType t);
    OrderType stringToOrderType(const std::string& s);

    enum Side {
        BUY,
        SELL,
        SCOUNT
    };
    std::string SideToString(Side t);
    Side stringToSide(const std::string& s);

    struct Order {
        long id;
        std::string symbol;
        double quantity;
        double price;
        OrderType type;
        Side side;
        double stopPrice;
        double icebergQty;
        long recvWindow;
        long emsId;
        std::string timeInForce;

        Order(long id, OrderType type, Side side, std::string symbol, double quantity, double price,
              double stopPrice=0., double icebergQty=0., long recvWindow=0, long emsId=0, std::string timeInForce="") {
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
        }
    };

    class OrderManager {
    private:
        std::queue<Order> mOrders;
        std::queue<Order> mPendingOrders;
        std::thread mOrderManagerThread;
        std::mutex mOrderCountMutex;
        std::mutex mOrderFetchMutex;
        bool mRunning;
        long mOrderCount;
        std::vector<Order> mSentOrders;
    public:
        OrderManager();
        ~OrderManager();
        void start();
        void run();
        void stop();
        bool isRunning();
        void createOrder(OrderType type, Side side, std::string symbol, double quantity, double price);
        void processOrder(Order order);
        void processOrders();

        bool hasOrders(); // returns whether there are orders to be sent
        Order& getOldestOrder(); // returns oldest order from mOrders
    private:
        int getNewOrderId();

    };

} // ats

#endif //ATS_ORDERMANAGER_H
