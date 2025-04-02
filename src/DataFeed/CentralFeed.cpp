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
    const auto topBid = static_cast<uint32_t>(ref->_bid[0]._price * 100);
    const auto topAsk = static_cast<uint32_t>(ref->_ask[0]._price * 100);
    const auto ltp    = static_cast<uint32_t>(ref->_lastTradePrice * 100);
    const auto atp    = static_cast<uint32_t>(ref->_averageTradePrice * 100);

    for (size_t i = 0; i < 5; ++i) {
        ref->_bid[i]._price    = static_cast<float>(pointer->_bid[i]._price) / 100.0F;
        ref->_bid[i]._quantity = (pointer->_bid[i]._quantity);
        ref->_bid[i]._order    = (pointer->_bid[i]._order);

        ref->_ask[i]._price    = static_cast<float>(pointer->_ask[i]._price) / 100.0F;
        ref->_ask[i]._quantity = (pointer->_ask[i]._quantity);
        ref->_ask[i]._order    = (pointer->_ask[i]._order);
    }

    ref->_totalBuyQuantity  = (pointer->_totalBuyQuantity);
    ref->_totalSellQuantity = (pointer->_totalSellQuantity);
    ref->_volumeTradedToday = (pointer->_volumeTradedToday);

    ref->_open  = static_cast<float>(pointer->_open) / 100.0F;
    ref->_high  = static_cast<float>(pointer->_high) / 100.0F;
    ref->_low   = static_cast<float>(pointer->_low) / 100.0F;
    ref->_close = static_cast<float>(pointer->_close) / 100.0F;

    ref->_averageTradePrice = static_cast<float>(pointer->_averageTradePrice) / 100.0F;
    ref->_lastTradePrice    = static_cast<float>(pointer->_lastTradePrice) / 100.0F;
    ref->_lastTradeQuantity = (pointer->_lastTradeQuantity);

    std::memset(ref->_lastTradeTime.data(), 0, 30);
    std::memcpy(ref->_lastTradeTime.data(), pointer->_lastTradeTime, 30);

    ref->_pchange = static_cast<float>(pointer->_close - pointer->_lastTradePrice) / static_cast<float>(pointer->_close) * 100.0F;

    if (topBid != pointer->_bid[0]._price) {
        ref->_color._topBid = topBid < (pointer->_bid[0]._price);
    }
    if (topAsk != pointer->_ask[0]._price) {
        ref->_color._topAsk = topAsk < (pointer->_ask[0]._price);
    }
    if (ltp != pointer->_lastTradePrice) {
        ref->_color._ltp = ltp < (pointer->_lastTradePrice);
    }
    if (atp != pointer->_averageTradePrice) {
        ref->_color._atp = atp < (pointer->_averageTradePrice);
    }
#if 0
    MarketEventQueue.push(ref);
#endif
}
