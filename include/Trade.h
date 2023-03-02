//
// Created by Anouar Achghaf on 02/03/2023.
//

#ifndef ATS_TRADE_H
#define ATS_TRADE_H

namespace ats {

    class Trade {
    private:
        long id;
        double price;
        double quantity;
        double quoteQty;
        long time;
        bool isBuyerMaker;
        bool isBestMatch;
    public:
        Trade(long id_, double price_, double quantity_, double quoteQty_, long time_,
                     bool isBuyerMaker_, bool isBestMatch_);
    };

} // ats

#endif //ATS_TRADE_H
