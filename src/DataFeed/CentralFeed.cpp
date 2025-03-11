#include "CentralFeed.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Structure.hpp"
#include <cstdint>

constexpr static int MarketWatchLadderCount = 5;
constexpr static int TimestampLength        = 50;

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
    float TopBid = ref->Bid[0].Price;
    float TopAsk = ref->Ask[0].Price;
    float LTP    = ref->LastTradePrice;
    float ATP    = ref->AverageTradePrice;

    for (int i = 0; i < 5; ++i) {
        ref->Bid[i].Price    = (pointer->_bid[i]._price) / 100.0f;
        ref->Bid[i].Quantity = (pointer->_bid[i]._quantity);
        ref->Bid[i].Order    = (pointer->_bid[i]._order);

        ref->Ask[i].Price    = (pointer->_ask[i]._price) / 100.0f;
        ref->Ask[i].Quantity = (pointer->_ask[i]._quantity);
        ref->Ask[i].Order    = (pointer->_ask[i]._order);
    }

    ref->TotalBuyQuantity  = (pointer->_totalBuyQuantity);
    ref->TotalSellQuantity = (pointer->_totalSellQuantity);
    ref->VolumeTradedToday = (pointer->_volumeTradedToday);

    ref->OpenPrice  = (pointer->_open) / 100.0f;
    ref->HighPrice  = (pointer->_high) / 100.0f;
    ref->LowPrice   = (pointer->_low) / 100.0f;
    ref->ClosePrice = (pointer->_close) / 100.0f;

    ref->AverageTradePrice = (pointer->_averageTradePrice) / 100.0f;
    ref->LastTradePrice    = (pointer->_lastTradePrice) / 100.0f;
    ref->LastTradeQuantity = (pointer->_lastTradeQuantity);

    std::memset(ref->LastTradeTime.data(), 0, 30);
    std::memcpy(ref->LastTradeTime.data(), pointer->_lastTradeTime, 30);

    ref->PercentageChange = ((float)(ref->ClosePrice - ref->LastTradePrice) / ref->ClosePrice) * 100;

    ref->Color.TopBid = TopBid > ref->Bid[0].Price;
    ref->Color.TopAsk = TopAsk > ref->Ask[0].Price;
    ref->Color.LTP    = LTP > ref->LastTradePrice;
    ref->Color.ATP    = ATP > ref->AverageTradePrice;

    MarketEventQueue.push(ref);
}
