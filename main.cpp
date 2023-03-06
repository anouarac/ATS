//
// Created by Anouar Achghaf on 12/02/2023.
//
#include <iostream>
#include "ats.h"
#include "json/json.h"
#include "binance_logger.h"
#include "Plotter.h"

int main(int argc, char const *argv[]) {
    binance::Logger::set_debug_level(1);
	binance::Logger::set_debug_logfp(stderr);
    ats::OrderManager oms;
    ats::BinanceExchangeManager bems(oms, 1);
    Json::Value result;
    //bems.getUserInfo(result);
//    std::cout << result.toStyledString() << std::endl;
//    std::cout << bems.getPrice("ETHUSDT");
    ImBinance app("ImBinance", 1280, 800, argc, argv, bems);
    app.Run();
    return 0;
}