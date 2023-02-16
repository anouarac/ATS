//
// Created by Anouar Achghaf on 16/02/2023.
//

#ifndef ATS_ORDERMANAGER_H
#define ATS_ORDERMANAGER_H
#include <string>
#include <queue>
#include <thread>
#include <mutex>

namespace ats {
    struct Order {
        int id;
        std::string symbol;
        double quantity;

        Order(int id, std::string symbol, double quantity) {
            this->id = id;
            this->symbol = symbol;
            this->quantity = quantity;
        }
    };

    class OrderManager {
    private:
        std::queue<Order> mOrders;
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
        void createOrder(std::string symbol, int quantity);
        void processOrder(Order order);
        void processOrders();
    private:
        int getNewOrderId();
    };

} // ats

#endif //ATS_ORDERMANAGER_H
