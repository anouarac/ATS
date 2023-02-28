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

namespace ats {
    enum OrderType {
        LIMIT,
        MARKET,
        STOP_LOSS,
        STOP_LOSS_LIMIT,
        TAKE_PROFIT,
        TAKE_PROFIT_LIMIT,
        LIMIT_MAKER
    };
    std::string OrderTypeToString(OrderType t);

    enum Side {
        BUY,
        SELL
    };
    std::string SideToString(Side t);

    struct Order {
        int id;
        std::string symbol;
        double quantity;
        double price;
        OrderType type;
        Side side;
        double stopPrice;
        double icebergQty;
        long recvWindow;

        Order(int id, OrderType type, Side side, std::string symbol, double quantity, double price,
              double stopPrice=0., double icebergQty=0., long recvWindow=0) {
            this->id = id;
            this->side = side;
            this->symbol = symbol;
            this->quantity = quantity;
            this->type = type;
            this->price = price;
            this->stopPrice = stopPrice;
            this->icebergQty = icebergQty;
            this->recvWindow = recvWindow;
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
        int mOrderCount;
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
    private:
        int getNewOrderId();

    };

} // ats

#endif //ATS_ORDERMANAGER_H
