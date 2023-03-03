//
// Created by Anouar Achghaf on 12/02/2023.
//
/**
 * @file ats.h
 * @author Anouar Achghaf
 * @date 12/02/2023
 * @brief Main header file for the ATS library
 */

#ifndef ALGO_TRADING_ATS_H
#define ALGO_TRADING_ATS_H

#include "MarketData.h"
#include "Strategy.h"
#include "PositionManager.h"
#include "OrderManager.h"
#include "ExchangeManager.h"
#include "BinanceExchangeManager.h"
#include "RiskManager.h"

/**
 * @namespace ats
 * @brief Namespace for the algorithmic trading system.
 *
 * This namespace contains all the classes and functions for the algorithmic trading system.
 * It includes classes for managing orders, trades, accounts, and exchanges, as well as
 * functions for analyzing market data, making trading decisions, and executing trades.
 *
 * The system is designed to be extensible, with different types of exchanges and strategies
 * able to be added through inheritance and polymorphism. The core functionality of the system
 * is provided by the ExchangeManager and StrategyManager classes, which coordinate the
 * interactions between exchanges, accounts, orders, and trades.
 *
 * The system uses the JSONCPP library for parsing and manipulating JSON data,
 * and currently supports the Binance exchange. It also includes a simulation mode for
 * testing strategies and algorithms without risking real funds using the Binance testnet.
 */

#endif //ALGO_TRADING_ATS_H
