/**
 * @file cancel_orders.cpp
 * @author Anouar Achghaf
 * @date 07/03/2023
 * @brief Cancelling all open orders
 */

#include "ats.h"
#include "binance_logger.h"
#include "Plotter.h"

using namespace ats;

int main(int argc, char const *argv[]) {
    binance::Logger::set_debug_level(1);
	binance::Logger::set_debug_logfp(stderr);
    OrderManager oms({"BTCUSDT", "ETHUSDT", "LTCUSDT"});
    BinanceExchangeManager ems(oms, 1);
    oms.cancelAllOrders();
    ImBinance app("ImBinance", 1280, 800, argc, argv, ems);
    app.Run();
    return 0;
}