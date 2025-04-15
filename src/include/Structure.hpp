#pragma once

#include "IconsMaterialDesign.h"
#include "Lancelot.hpp"
#include "imgui.h"

#include <boost/lockfree/spsc_queue.hpp>

#include <deque>
#include <list>
#include <map>
#include <memory>

static_assert(true);
#pragma pack(push, 1)

enum DataType : int;
enum OrderStatus : int;
enum StrategyStatus : int;
enum OrderType : int;

namespace Lancelot {
    enum Side : uint8_t;
    enum Exchange : uint8_t;
    struct ResultSetT;
    using ResultSetPtrT = const ResultSetT*;
}  // namespace Lancelot

using TokenT    = int32_t;
using QuantityT = int32_t;
using PriceT    = float;

constexpr int MarketWatchLadderCount = 5;
constexpr int StrategyNameLength     = 50;

using PricePointsT = struct PricePointsT {
    PriceT    _price;
    QuantityT _quantity;
    QuantityT _order;
};

using ColorBitT = struct ColorBitT {
    bool _topBid;
    bool _topAsk;
    bool _ltp;
    bool _atp;
};

using MarketWatchDataT = struct MarketWatchDataT {
    std::array<PricePointsT, MarketWatchLadderCount> _bid;
    std::array<PricePointsT, MarketWatchLadderCount> _ask;
    std::array<char, StrategyNameLength>             _lastTradeTime;
    std::array<char, StrategyNameLength>             _description;

    TokenT    _token;
    QuantityT _lastTradeQuantity;

    PriceT _averageTradePrice;
    PriceT _lastTradePrice;
    PriceT _lowDpr;
    PriceT _highDpr;

    PriceT _open;
    PriceT _high;
    PriceT _low;
    PriceT _close;
    PriceT _pchange;

    QuantityT _totalBuyQuantity;
    QuantityT _totalSellQuantity;
    QuantityT _volumeTradedToday;
    QuantityT _openInterest;

    ColorBitT _color;
};

using MarketWatchDataPtrT = std::shared_ptr<MarketWatchDataT>;

using ParameterValueT = struct ParameterValueT {
    bool        _check{false};
    int         _integer{0};
    float       _floating{0.0F};
    std::string _text;
};

using ParameterInfoT = struct ParameterInfoT {
    bool                _searchEnable{false};
    DataType            _type;
    ParameterValueT     _parameter;
    ImGuiTextFilter     _filter;
    MarketWatchDataPtrT _marketWatch;
};

using GlobalParameterInfoT = struct GlobalParameterInfoT {
    bool           _update;
    std::string    _name;
    ParameterInfoT _parameterInfo;
};

using OrderInfoT = struct OrderInfoT {
    uint32_t _portfolio;
    uint32_t _uniqueId;
    uint32_t _token;
    uint32_t _quantity;
    uint32_t _fillQuantity;
    uint32_t _remaining;
    long     _orderNumber;
    float    _price;
    float    _fillPrice;

    Lancelot::Side _side;

    OrderStatus _statusValue;
    std::string _contract;
    std::string _time;
    std::string _client;
    std::string _message;
};

using BookInfoT = struct BookInfoT {
    PriceT  _price;
    PriceT  _average;
    int32_t _quantity;
};
using NetBookColumnT = struct NetBookColumnT {
    uint32_t _portfolio;
    uint32_t _buyQuantity;
    uint32_t _sellQuantity;
    int32_t  _totalQuantity;

    float _lastTradePrice;
    float _averageBuyPrice;
    float _averageSellPrice;
    float _netInvestment;
    float _totalBuyPrice;
    float _totalSellPrice;

    float _lastPNL;
    float _pnl;
    float _mtm;

    MarketWatchDataPtrT _marketWatch;
};

using GreeksT = struct GreeksT {
    bool     _isCall;
    bool     _isFuture;
    uint32_t _expiry;
    float    _strike;
    float    _impliedVol;
    float    _delta;
    float    _gamma;
    float    _vega;
    float    _theta;

    MarketWatchDataPtrT _marketWatch;
    MarketWatchDataPtrT _future;
};
using GreeksPtrT = std::shared_ptr<GreeksT>;

using GreekBookColumnT = struct GreekBookColumnT {
    std::string _symbol;
    uint32_t    _buyQuantity;
    uint32_t    _sellQuantity;

    float _totalSellPrice;
    float _totalBuyPrice;
    float _averageSellPrice;
    float _averageBuyPrice;

    GreeksPtrT _greeks;
};

using DValueT = struct DValueT {
    float _delta;
    float _gamma;
    float _vega;
    float _theta;
    float _mtm;
    float _ltp;
    float _value;

    std::string _symbol;
};

using PortfolioStatusT = struct PortfolioStatusT {
    bool     _close;
    uint32_t _inactive;
    uint32_t _active;
    uint32_t _apply;
    uint32_t _waiting;
    uint32_t _terminate;
};

using OrderFormInfoT = struct OrderFormInfoT {
    uint32_t       _uniqueId;
    PriceT         _price;
    QuantityT      _quantity;
    QuantityT      _lotSize;
    long           _orderNumber;
    int            _type;
    Lancelot::Side _side;
    OrderStatus    _status;
    std::string    _contract;
    std::string    _client;

    MarketWatchDataPtrT _marketWatch;
};

using SpotInfoT = struct SpotInfoT {
    float _value;
    float _change;
};

using ParameterInfoListT = std::map<std::string, ParameterInfoT>;

using StrategyRowT = struct StrategyRowT {
    bool               _changed;
    bool               _subscribed;
    bool               _selected;
    uint32_t           _portfolio;
    StrategyStatus     _status;
    ParameterInfoListT _parameterInfoList;
};

using OptionChainItemT = struct OptionChainItemT {
    MarketWatchDataPtrT     _marketWatch;
    Lancelot::ResultSetPtrT _contract;
};

using OptionChainRowT = struct OptionChainRowT {
    OptionChainItemT _call;
    OptionChainItemT _put;
};

using ExcelContactItemT = struct ExcelContactItemT {
    size_t              _indea;
    MarketWatchDataPtrT _marketWatch;
};

using ScannerFunctionInfoT = struct ScannerFunctionInfoT {
    bool        _selected;
    char        _variable;
    std::string _name;
};

using ClientInfoT = struct ClientInfoT {
    Lancelot::Exchange _exchange;
    std::string        _clientCode;
};

using TradeTrackerItemT = struct TradeTrackerItemT {
    int         _portfolio;
    std::string _strategy;
    std::string _descriptions;
};

using SaveScannerItemT = struct SaveScannerItemT {
    bool        _applied;
    int64_t     _uniqueID;
    std::string _name;
    std::string _expandedEquation;
};

class Portfolio;
using PortfolioPtrT = std::shared_ptr<Portfolio>;

using ScannerResultOutputT = struct ScannerResultOutputT {
    PortfolioPtrT      _portfolio;
    ParameterInfoListT _parameterInfoList;
};
#pragma pack(pop)

struct Singleton {
    Singleton()                                    = default;
    Singleton(const Singleton&)                    = delete;
    Singleton(Singleton&&)                         = delete;
    auto operator=(const Singleton&) -> Singleton& = delete;
    auto operator=(Singleton&&) -> Singleton&      = delete;
};

class ExcelAutomation;
using ExcelAutomationPtrT = std::unique_ptr<ExcelAutomation>;
using StrategyRowPtrT     = std::shared_ptr<StrategyRowT>;
using WeakStrategyRowPtrT = std::weak_ptr<StrategyRowT>;
using NetBookColumnPtrT   = std::shared_ptr<NetBookColumnT>;
using GreekBookColumnPtrT = std::shared_ptr<GreekBookColumnT>;
using OrderInfoPtrT       = std::shared_ptr<OrderInfoT>;

using MarketWatchDatContainerT  = std::unordered_map<uint32_t, MarketWatchDataPtrT>;
using GlobalStrategyListT       = std::unordered_map<uint32_t, WeakStrategyRowPtrT>;
using GreekBookContainerT       = std::unordered_map<uint32_t, GreekBookColumnPtrT>;
using SymbolWiseTradeContainerT = std::unordered_map<uint32_t, NetBookColumnPtrT>;

using PFWiseTradeContainerT      = std::map<std::pair<uint32_t, uint32_t>, NetBookColumnPtrT>;
using ScannerInfoFromDatabaseT   = std::map<int, std::string>;
using ExcelContactItemContainerT = std::map<std::string, ExcelContactItemT>;

using GlobalParamListT              = std::vector<GlobalParameterInfoT>;
using AllContractT                  = std::vector<std::string>;
using StrategyNameListT             = AllContractT;
using ClientCodeListT               = std::vector<ClientInfoT>;
using StrategyListT                 = std::deque<StrategyRowPtrT>;
using ScannerFunctionListContainerT = std::vector<ScannerFunctionInfoT>;
using TradeTrackerContainerT        = std::deque<TradeTrackerItemT>;
using ScannerSaveContainerT         = std::vector<SaveScannerItemT>;
using SymbolWiseTradeContainerVecT  = std::deque<std::pair<int, NetBookColumnPtrT>>;
using PFWiseTradeContainerVecT      = std::deque<std::pair<std::pair<int, int>, NetBookColumnPtrT>>;
using BookOrderListT                = std::deque<OrderInfoPtrT>;

using GlobalPortfolioScannerContainerT = std::unordered_map<int64_t, ScannerResultOutputT>;

using GreeksListT = std::list<GreeksPtrT>;

template <typename Type, size_t Size = 30000>
using QueueT                = boost::lockfree::spsc_queue<Type, boost::lockfree::capacity<Size>>;
using PendingTradeUpdateT   = QueueT<OrderInfoPtrT>;
using PendingTrackerUpdateT = QueueT<TradeTrackerItemT>;
using MarketEventQueueT     = QueueT<MarketWatchDataPtrT>;
using ScannerAddQueueT      = QueueT<StrategyRowPtrT>;
