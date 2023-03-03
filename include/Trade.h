/**
 * @file Trade.h
 * @author Anouar Achghaf
 * @date 02/03/2023
 * @brief Contains the Trade class definition
*/

#ifndef ATS_TRADE_H
#define ATS_TRADE_H

namespace ats {

    /**
     * @brief Class representing a trade executed on an exchange
     */
    class Trade {
    private:
        long id; /**< Trade ID */
        double price; /**< Trade price */
        double quantity; /**< Trade quantity */
        double quoteQty; /**< Trade quote quantity */
        long time; /**< Trade execution time */
        bool isBuyerMaker; /**< Flag indicating whether the buyer is the maker */
        bool isBestMatch; /**< Flag indicating whether the trade was the best price match at the time */
    public:
        /**
         * @brief Constructor for Trade class
         * @param id_ Trade ID
         * @param price_ Trade price
         * @param quantity_ Trade quantity
         * @param quoteQty_ Trade quote quantity
         * @param time_ Trade execution time
         * @param isBuyerMaker_ Flag indicating whether the buyer is the maker
         * @param isBestMatch_ Flag indicating whether the trade was the best price match at the time
         */
        Trade(long id_, double price_, double quantity_, double quoteQty_, long time_,
              bool isBuyerMaker_, bool isBestMatch_);
    };


} // ats

#endif //ATS_TRADE_H
