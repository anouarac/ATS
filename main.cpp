//
// Created by Anouar Achghaf on 12/02/2023.
//
#include <iostream>
#include "ats.h"
#include "json/json.h"
#include "binance_logger.h"

int main() {
    binance::Logger::set_debug_level(1);
	binance::Logger::set_debug_logfp(stderr);
    ats::OrderManager oms;
    ats::BinanceExchangeManager bems(oms, 1);
    ats::OrderType t = ats::MARKET;
    std::cout << ats::OrderTypeToString(t);
    return 0;
}