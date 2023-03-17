/**
 * @file strategy.cpp
 * @author Anouar Achghaf
 * @date 07/03/2023
 * @brief Run a strategy
 */

#include "ats.h"
#include "binance_logger.h"
#include "Plotter.h"
#include <numeric>
#include <ctime>

using namespace ats;

class ExampleStrategy : public Strategy {
public:
    /**
     * @brief Constructs an ExampleStrategy object
     * @param symbol The symbol the strategy will trade
     * @param data MarketData object to get market information
     * @param orderManager OrderManager object to create and manage orders
     * @param prices Vector of historical prices (default empty)
     */
    ExampleStrategy(std::string symbol, MarketData &data, OrderManager &orderManager, std::vector<double> prices = {})
            : Strategy(symbol, data, orderManager, prices) {}

private:
    std::map<std::string, double> mBalances;
    time_t mLastOrder{0};
    time_t mDataFetch{0};
public:
    ~ExampleStrategy() {
        mOrderManager.cancelAllOrders();
        stop();
    }

    virtual void updatePrice() override {
        double currentPrice = mData.getPrice(mSymbol);
        time_t new_t;
        time(&new_t);
        if (difftime(new_t, mDataFetch) > 1) {
            mPrices.push_back(currentPrice);
            time(&mDataFetch);
        }
    }

    virtual double getSignal() override {
        updatePrice();

        double shortSMA = 0.0, longSMA = 0.0;
        int shortPeriod = 10, longPeriod = 50;
        if (mPrices.size() >= shortPeriod) {
            shortSMA = std::accumulate(mPrices.end() - shortPeriod, mPrices.end(), 0.0) / shortPeriod;
        }
        if (mPrices.size() >= longPeriod) {
            longSMA = std::accumulate(mPrices.end() - longPeriod, mPrices.end(), 0.0) / longPeriod;
        }

        // Check for a crossover between the short and long SMAs
        double signal = 0;
        if (shortSMA > longSMA && mPrices.size() > longPeriod)
            signal = 1;
        else if (shortSMA < longSMA && mPrices.size() > longPeriod)
            signal = -1;

        return signal;
    }

    void updateBalance() {
        mBalances = mData.getBalances();
    }

    virtual void buy() override {
        time_t curTime(0);
        time(&curTime);
        if (difftime(curTime, mLastOrder) < 15)
            return;
        updateBalance();
        mOrderManager.createOrder(Order(0, LIMIT, BUY, mSymbol,
                                        floor(mData.getQtyForPrice(mSymbol, 0.01 * mBalances["USDT"]) * 1e6) / 1e6,
                                        mPrices.back(), 0, 0, 0, 0, "GTC", 0));
        time(&mLastOrder);
    }

    virtual void sell() override {
        time_t curTime(0);
        time(&curTime);
        if (difftime(curTime, mLastOrder) < 15)
            return;
        updateBalance();
        mOrderManager.createOrder(Order(0, LIMIT, SELL, mSymbol,
                                  floor(mData.getQtyForPrice(mSymbol, 0.01 * mBalances["USDT"]) * 1e6) / 1e6,
                                  mPrices.back(), 0, 0, 0, 0, "GTC", 0));
        time(&mLastOrder);
    }
};

int main(int argc, char const *argv[]) {
    binance::Logger::set_debug_level(1);
    binance::Logger::set_debug_logfp(stderr);
    OrderManager oms;
    BinanceExchangeManager ems(oms, 1);
    MarketData md({"BTCUSDT", "ETHUSDT"}, ems);
    ExampleStrategy strat("BTCUSDT", md, oms, md.getPrices("BTCUSDT"));
    ImBinance app("ImBinance", 1280, 800, argc, argv, ems);
    app.Run();
    return 0;
}