#include "CentralFeed.hpp"

#include "../API/ContractInfo.hpp"
#include "../include/Structure.hpp"
#include "Structure.hpp"

#include <boost/algorithm/string/trim.hpp>

constexpr static int TimestampLength = 50;

using PriceCeT = uint32_t;

#pragma pack(push, 1)
struct PricePointsCET {
    PriceCeT  _price{};
    QuantityT _quantity{};
    QuantityT _order{};
};
struct IndexDataT {
    PriceCeT _value;
    PriceCeT _open;
    PriceCeT _high;
    PriceCeT _low;
    PriceCeT _close;
    PriceCeT _yearlyHigh;
    PriceCeT _yearlyLow;
    float    _percentageChange;
    char     _name[21];
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

struct MarketWatchDataUpdateT {
    Lancelot::Header        _header;
    MarketWatchDataCentralT _data;
};

struct IndexDataUpdate {
    Lancelot::Header _header;
    IndexDataT       _data;
};

#pragma pack(pop)
extern SpotInfoT BankNifty;
extern SpotInfoT Nifty;
extern SpotInfoT VIX;

void CentralFeed::Process(int size_) {
    LOG(INFO, "{} {} {} {}", __FUNCTION__, size_, _header->_length, _header->_type);
    switch (_header->_type) {
        case 4: {
            const auto current  = _marketData->_data;
            const auto previous = ContractInfo::GetLiveDataRef(current._token);

            if (not previous) {
                return;
            }
            const auto topBid = static_cast<PriceCeT>(previous->_bid[0]._price * 100);
            const auto topAsk = static_cast<PriceCeT>(previous->_ask[0]._price * 100);
            const auto ltp    = static_cast<PriceCeT>(previous->_lastTradePrice * 100);
            const auto atp    = static_cast<PriceCeT>(previous->_averageTradePrice * 100);

            for (size_t i = 0; i < 5; ++i) {
                previous->_bid[i]._price    = static_cast<PriceT>(current._bid[i]._price) / 100.0F;
                previous->_bid[i]._quantity = (current._bid[i]._quantity);
                previous->_bid[i]._order    = (current._bid[i]._order);

                previous->_ask[i]._price    = static_cast<PriceT>(current._ask[i]._price) / 100.0F;
                previous->_ask[i]._quantity = (current._ask[i]._quantity);
                previous->_ask[i]._order    = (current._ask[i]._order);
            }

            previous->_lastTradeQuantity = (current._lastTradeQuantity);
            previous->_totalBuyQuantity  = (current._totalBuyQuantity);
            previous->_totalSellQuantity = (current._totalSellQuantity);
            previous->_volumeTradedToday = (current._volumeTradedToday);

            previous->_open              = static_cast<PriceT>(current._open) / 100.0F;
            previous->_high              = static_cast<PriceT>(current._high) / 100.0F;
            previous->_low               = static_cast<PriceT>(current._low) / 100.0F;
            previous->_close             = static_cast<PriceT>(current._close) / 100.0F;
            previous->_averageTradePrice = static_cast<PriceT>(current._averageTradePrice) / 100.0F;
            previous->_lastTradePrice    = static_cast<PriceT>(current._lastTradePrice) / 100.0F;
            previous->_openInterest      = current._openInterest;

            previous->_lowDpr  = static_cast<PriceT>(current._lowDPR) / 100.0F;
            previous->_highDpr = static_cast<PriceT>(current._highDPR) / 100.0F;

            memset(previous->_lastTradeTime.data(), 0, 30);
            memcpy(previous->_lastTradeTime.data(), current._lastTradeTime, 30);

            previous->_pchange = current._percentageChange;

            if (topBid != current._bid[0]._price) {
                previous->_color._topBid = topBid < (current._bid[0]._price);
            }
            if (topAsk != current._ask[0]._price) {
                previous->_color._topAsk = topAsk < (current._ask[0]._price);
            }
            if (ltp != current._lastTradePrice) {
                previous->_color._ltp = ltp < (current._lastTradePrice);
            }
            if (atp != current._averageTradePrice) {
                previous->_color._atp = atp < (current._averageTradePrice);
            }
#if 0
    _marketEventQueue.push(previous);
#endif
            break;
        }
        case 5: {
            std::string name = std::string(_index->_data._name, 21);
            boost::algorithm::trim(name);
            LOG(INFO, "{} - value {}  change {} ", name, _index->_data._value, _index->_data._percentageChange);
            if (name == "Nifty Bank") {
                BankNifty._value  = _index->_data._value / 100.0F;
                BankNifty._change = _index->_data._percentageChange;
            } else if (name == "Nifty 50") {
                Nifty._value  = _index->_data._value / 100.0F;
                Nifty._change = _index->_data._percentageChange;
            } else if (name == "India VIX") {
                VIX._value  = _index->_data._value / 100.F;
                VIX._change = _index->_data._percentageChange;
            }
            break;
        }
    }
}
CentralFeed::CentralFeed(MarketEventQueueT& queue_)
    : _marketEventQueue(queue_),
      _marketData(reinterpret_cast<MarketWatchDataUpdateT*>(_buffer)),
      _index(reinterpret_cast<IndexDataUpdate*>(_buffer)),
      _header(reinterpret_cast<Lancelot::Header*>(_buffer)) {
}
