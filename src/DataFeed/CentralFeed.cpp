#include "CentralFeed.hpp"

#include "../API/ContractInfo.hpp"
#include "../include/Structure.hpp"

#include <cstdint>

constexpr static int TimestampLength = 50;

using PriceCeT = uint32_t;

extern MarketEventQueueT MarketEventQueue;

#pragma pack(push, 1)
struct PricePointsCET {
    PriceCeT  _price{};
    QuantityT _quantity{};
    QuantityT _order{};
};

struct MarketWatchDataCentralT {
    PricePointsCET _bid[MarketWatchLadderCount]{};
    PricePointsCET _ask[MarketWatchLadderCount]{};

    TokenT    _token{};
    QuantityT _lastTradeQuantity{};
    PriceCeT  _averageTradePrice{};
    PriceCeT  _lastTradePrice{};
    PriceCeT  _lowDPR{};
    PriceCeT  _highDPR{};
    PriceCeT  _lowLPP{};
    PriceCeT  _highLPP{};
    PriceCeT  _open{};
    PriceCeT  _high{};
    PriceCeT  _low{};
    PriceCeT  _close{};
    float     _percentageChange{};

    QuantityT _totalBuyQuantity{};
    QuantityT _totalSellQuantity{};
    QuantityT _volumeTradedToday{};
    QuantityT _openInterest{};

    char _lastTradeTime[TimestampLength]{};
};

#pragma pack(pop)

void CentralFeed::Process(const char* buffer_, size_t /*size_*/) {
    const auto* current  = reinterpret_cast<const MarketWatchDataCentralT*>(buffer_);
    const auto  previous = ContractInfo::GetLiveDataRef(current->_token);

    if (not previous) {
        return;
    }
    const auto topBid = static_cast<PriceCeT>(previous->_bid[0]._price * 100);
    const auto topAsk = static_cast<PriceCeT>(previous->_ask[0]._price * 100);
    const auto ltp    = static_cast<PriceCeT>(previous->_lastTradePrice * 100);
    const auto atp    = static_cast<PriceCeT>(previous->_averageTradePrice * 100);

    for (size_t i = 0; i < 5; ++i) {
        previous->_bid[i]._price    = static_cast<PriceT>(current->_bid[i]._price) / 100.0F;
        previous->_bid[i]._quantity = (current->_bid[i]._quantity);
        previous->_bid[i]._order    = (current->_bid[i]._order);

        previous->_ask[i]._price    = static_cast<PriceT>(current->_ask[i]._price) / 100.0F;
        previous->_ask[i]._quantity = (current->_ask[i]._quantity);
        previous->_ask[i]._order    = (current->_ask[i]._order);
    }

    previous->_lastTradeQuantity = (current->_lastTradeQuantity);
    previous->_totalBuyQuantity  = (current->_totalBuyQuantity);
    previous->_totalSellQuantity = (current->_totalSellQuantity);
    previous->_volumeTradedToday = (current->_volumeTradedToday);

    previous->_open              = static_cast<PriceT>(current->_open) / 100.0F;
    previous->_high              = static_cast<PriceT>(current->_high) / 100.0F;
    previous->_low               = static_cast<PriceT>(current->_low) / 100.0F;
    previous->_close             = static_cast<PriceT>(current->_close) / 100.0F;
    previous->_averageTradePrice = static_cast<PriceT>(current->_averageTradePrice) / 100.0F;
    previous->_lastTradePrice    = static_cast<PriceT>(current->_lastTradePrice) / 100.0F;

    memset(previous->_lastTradeTime.data(), 0, 30);
    memcpy(previous->_lastTradeTime.data(), current->_lastTradeTime, 30);

    previous->_pchange = (static_cast<PriceT>(current->_close - current->_lastTradePrice) / static_cast<PriceT>(current->_close)) * 100.0F;

    if (topBid != current->_bid[0]._price) {
        previous->_color._topBid = topBid < (current->_bid[0]._price);
    }
    if (topAsk != current->_ask[0]._price) {
        previous->_color._topAsk = topAsk < (current->_ask[0]._price);
    }
    if (ltp != current->_lastTradePrice) {
        previous->_color._ltp = ltp < (current->_lastTradePrice);
    }
    if (atp != current->_averageTradePrice) {
        previous->_color._atp = atp < (current->_averageTradePrice);
    }
#if 0
    MarketEventQueue.push(ref);
#endif
}
