/**
 * @file exchange_ui.cpp
 * @author Anouar Achghaf
 * @date 07/03/2023
 * @brief Launch Exchange UI
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
    ImBinance app("ImBinance", 1280, 800, argc, argv, ems);
    app.Run();
    return 0;
}