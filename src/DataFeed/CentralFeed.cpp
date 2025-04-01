#include "CentralFeed.hpp"

#include "../API/ContractInfo.hpp"
#include "../include/Structure.hpp"

#include <cstdint>

constexpr static int TimestampLength = 50;

using TokenT    = uint32_t;
using PriceT    = uint32_t;
using QuantityT = uint32_t;

extern MarketEventQueueT MarketEventQueue;

#pragma pack(push, 1)
struct MarketWatchDataCentralT {
    struct PricePointsT {
        PriceT    _price{};
        QuantityT _quantity{};
        uint32_t  _order{};
    };
    PricePointsT _bid[MarketWatchLadderCount]{};
    PricePointsT _ask[MarketWatchLadderCount]{};

    TokenT    _token{};
    QuantityT _lastTradeQuantity{};
    PriceT    _averageTradePrice{};
    PriceT    _lastTradePrice{};

    PriceT _lowDPR{};
    PriceT _highDPR{};

    PriceT _lowLPP{};
    PriceT _highLPP{};

    PriceT _open{};
    PriceT _high{};
    PriceT _low{};
    PriceT _close{};
    float  _percentageChange{};

    QuantityT _totalBuyQuantity{};
    QuantityT _totalSellQuantity{};
    QuantityT _volumeTradedToday{};
    QuantityT _openInterest{};

    char _lastTradeTime[TimestampLength]{};
};

#pragma pack(pop)
void CentralFeed::Process(char* buffer, size_t size_) {
    const auto* pointer = reinterpret_cast<MarketWatchDataCentralT*>(buffer);
    const auto  ref     = ContractInfo::GetLiveDataRef(pointer->_token);
    if (not ref) {
        return;
    }
    float TopBid = ref->_bid[0]._price;
    float TopAsk = ref->_ask[0]._price;
    float LTP    = ref->_lastTradePrice;
    float ATP    = ref->_averageTradePrice;

    for (int i = 0; i < 5; ++i) {
        ref->_bid[i]._price    = (pointer->_bid[i]._price) / 100.0f;
        ref->_bid[i]._quantity = (pointer->_bid[i]._quantity);
        ref->_bid[i]._order    = (pointer->_bid[i]._order);

        ref->_ask[i]._price    = (pointer->_ask[i]._price) / 100.0f;
        ref->_ask[i]._quantity = (pointer->_ask[i]._quantity);
        ref->_ask[i]._order    = (pointer->_ask[i]._order);
    }

    ref->_totalBuyQuantity  = (pointer->_totalBuyQuantity);
    ref->_totalSellQuantity = (pointer->_totalSellQuantity);
    ref->_volumeTradedToday = (pointer->_volumeTradedToday);

    ref->_open  = (pointer->_open) / 100.0f;
    ref->_high  = (pointer->_high) / 100.0f;
    ref->_low   = (pointer->_low) / 100.0f;
    ref->_close = (pointer->_close) / 100.0f;

    ref->_averageTradePrice = (pointer->_averageTradePrice) / 100.0f;
    ref->_lastTradePrice    = (pointer->_lastTradePrice) / 100.0f;
    ref->_lastTradePrice    = (pointer->_lastTradeQuantity);

    std::memset(ref->_lastTradeTime.data(), 0, 30);
    std::memcpy(ref->_lastTradeTime.data(), pointer->_lastTradeTime, 30);

    ref->_pchange = ((float)(ref->_close - ref->_lastTradePrice) / ref->_close) * 100;

    ref->_color._topBid = TopBid > ref->_bid[0]._price;
    ref->_color._topAsk = TopAsk > ref->_ask[0]._price;
    ref->_color._ltp    = LTP > ref->_lastTradePrice;
    ref->_color._atp    = ATP > ref->_averageTradePrice;
#if 0
    MarketEventQueue.push(ref);
#endif
}
