/**
 * @file reset_testnet_balance.cpp
 * @author Anouar Achghaf
 * @date 15/03/2023
 * @brief Reset test net balance to closer to original
 */

#include "ats.h"
#include "binance_logger.h"
#include "Plotter.h"

using namespace ats;

int main(int argc, char const *argv[]) {
    binance::Logger::set_debug_level(1);
    binance::Logger::set_debug_logfp(stderr);
    OrderManager oms;
    BinanceExchangeManager ems(oms, 1);
    auto balances = ems.getBalances();
//    oms.createOrder(Order(0, ats::MARKET, ats::SELL, "BTCUSDT", balances["BTC"]-1, 0));
//    oms.createOrder(Order(0, ats::MARKET, ats::BUY, "BTCUSDT", floor((balances["USDT"]-10000)/ems.getPrice("BTCUSDT")*1e6)/1e6, 0));
//    oms.createOrder(Order(0, ats::MARKET, ats::SELL, "BTCBUSD", floor((balances["BTC"]-1)*1e6)/1e6, 0));
    ImBinance app("ImBinance", 1280, 800, argc, argv, ems);
    app.Run();
    return 0;
}