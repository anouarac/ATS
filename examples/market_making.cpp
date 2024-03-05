/**
 * @file market_making.cpp
 * @author Anouar Achghaf
 * @date 15/03/2023
 * @brief Market making strategy
 */

#include "ats.h"
#include "binance_logger.h"
#include "Plotter.h"
#include <numeric>
#include <ctime>

using namespace ats;

class MMStrategy : public Strategy {
public:
    /**
     * @brief Constructs an ExampleStrategy object
     * @param symbol The symbol the strategy will trade
     * @param data MarketData object to get market information
     * @param orderManager OrderManager object to create and manage orders
     * @param prices Vector of historical prices (default empty)
     */
    MMStrategy(std::string symbol, MarketData &data, OrderManager &orderManager,
               std::string hedgeSymbol)
            : Strategy(symbol, data, orderManager, {}), mHedgeSymbol(hedgeSymbol) {}

private:
    std::map<std::string, double> mBalances;
    std::string mHedgeSymbol;
    std::vector<double> mHedgePrices;
    time_t mLastOrder{0};
    time_t mDataFetch{0};
public:
    ~MMStrategy() {
        mOrderManager.cancelAllOrders();
        stop();
    }

    virtual void updatePrice() override {
        double currentPrice = mData.getPrice(mSymbol);
        double hedgePrice = mData.getPrice(mHedgeSymbol);
        time_t new_t;
        time(&new_t);
        if (difftime(new_t, mDataFetch) > 1) {
            mPrices.push_back(currentPrice);
            mHedgePrices.push_back(hedgePrice);
            time(&mDataFetch);
        }
    }

    virtual double getSignal() override {
        updatePrice();
        auto ob1 = mData.getOrderBook(mSymbol);
        auto ob2 = mData.getOrderBook(mHedgeSymbol);
        if (ob1.bid.empty() || ob2.bid.empty() || ob1.ask.empty() || ob2.ask.empty())
            return 0;
        if (ob1.bid[0] > ob2.ask[0])
            return 1;
        else if (ob2.bid[0] > ob1.ask[0])
            return -1;
        return 0;
    }

    void updateBalance() {
        mBalances = mData.getBalances();
    }

    virtual void buy() override {
        time_t curTime(0);
        time(&curTime);
        if (difftime(curTime, mLastOrder) < 5)
            return;
        updateBalance();
        double quantity = floor(mBalances["BTC"]*0.01 * 1e6) / 1e6;
        quantity = std::min(quantity, floor(mBalances["USDT"]*0.01/mHedgePrices.back()*1e6)/1e6);
        if (quantity < 1e-6) return;
        OrderBook ob1 = mData.getOrderBook(mSymbol);
        OrderBook ob2 = mData.getOrderBook(mHedgeSymbol);
        quantity = std::min(quantity, ob1.bidVol[0]);
        double price = ob1.bid[0];
        if (price*quantity < 10) return;
        mOrderManager.setLastOrderQty(-1);
        Order order = Order(0, LIMIT, SELL, mSymbol,
                                        quantity,
                                        price, 0, 0, 0, 0, "IOC", 0);
        mOrderManager.createOrder(order);
        while (mOrderManager.getLastOrderQty() == -1) continue;
        double qty = mOrderManager.getLastOrderQty();
        mOrderManager.setLastOrderQty(-1);
        order = Order(0, LIMIT, BUY, mHedgeSymbol, qty, ob2.ask[0], 0, 0, 0, 0, "GTC", 0);
        if (qty > 1e-7)
            mOrderManager.createOrder(order);
        time(&mLastOrder);
    }

    virtual void sell() override {
        time_t curTime(0);
        time(&curTime);
        if (difftime(curTime, mLastOrder) < 5)
            return;
        updateBalance();
        double quantity = floor(mBalances["BTC"]*0.01 * 1e6) / 1e6;
        quantity = std::min(quantity, floor(mBalances["BUSD"]*0.01/mPrices.back()*1e6)/1e6);
        if (quantity < 1e-6) return;
        OrderBook ob1 = mData.getOrderBook(mSymbol);
        OrderBook ob2 = mData.getOrderBook(mHedgeSymbol);
        quantity = std::min(quantity, ob1.bidVol[0]);
        double price = ob1.ask[0];
        if (price*quantity < 10) return;
        mOrderManager.setLastOrderQty(-1);
        Order order = Order(0, LIMIT, BUY, mSymbol,
                                        quantity,
                                        price, 0, 0, 0, 0, "IOC", 0);
        mOrderManager.createOrder(order);
        while (mOrderManager.getLastOrderQty() == -1) continue;
        double qty = mOrderManager.getLastOrderQty();
        mOrderManager.setLastOrderQty(-1);
        order = Order(0, LIMIT, SELL, mHedgeSymbol, qty, ob2.bid[0], 0, 0, 0, 0, "GTC", 0);
        if (qty > 1e-7)
            mOrderManager.createOrder(order);
        time(&mLastOrder);
    }
};

int main(int argc, char const *argv[]) {
    binance::Logger::set_debug_level(1);
    binance::Logger::set_debug_logfp(stderr);
    OrderManager oms;
    BinanceExchangeManager ems(oms, 1, 1);
    MarketData md({"BTCUSDT", "BTCBUSD"}, ems, 1);
    MMStrategy strat("BTCBUSD", md, oms, "BTCUSDT");
    ImBinance app("ImBinance", 1280, 800, argc, argv, ems);
    app.Run();
    return 0;
}