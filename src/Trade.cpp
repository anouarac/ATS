//
// Created by Anouar Achghaf on 02/03/2023.
//

#include "Trade.h"

namespace ats {


    Trade::Trade(long id_, double price_, double quantity_, double quoteQty_, long time_,
                               bool isBuyerMaker_, bool isBestMatch_) {
        id = id_;
        price = price_;
        quantity = quantity_;
        quoteQty = quoteQty_;
        time = time_;
        isBuyerMaker = isBuyerMaker_;
        isBestMatch = isBestMatch_;
    }
} // ats