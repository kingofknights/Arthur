//
// Created by VIKLOD on 22-01-2023.
//

#pragma once

// #define DISABLE_LOGGING
#include <Lancelot.hpp>
#include <array>
#include <boost/lockfree/spsc_queue.hpp>
#include <cstdint>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "IconsMaterialDesign.h"
#include "imgui.h"

#pragma pack(push, 1)

enum DataType : int;
enum OrderStatus : int;
enum StrategyStatus : int;
enum OrderType : int;

constexpr int MarketWatchLadderCount = 5;
constexpr int StrategyNameLength     = 50;

using PricePointsT = struct PricePointsT {
    double   _price;
    uint32_t _quantity;
    uint32_t _order;
};

using ColorBitT = struct ColorBitT {
    uint8_t _topBid : 1;
    uint8_t _topAsk : 1;
    uint8_t _ltp : 1;
    uint8_t _atp : 1;
    uint8_t _dummy : 4;
};

using MarketWatchDataT = struct MarketWatchDataT {
    std::array<PricePointsT, MarketWatchLadderCount> _bid;
    std::array<PricePointsT, MarketWatchLadderCount> _ask;
    std::array<char, StrategyNameLength>             _lastTradeTime;
    std::array<char, StrategyNameLength>             _description;

    uint32_t _token;
    uint32_t _ltq;
    double   _atp;
    double   _ltp;
    double   _lowDpr;
    double   _highDpr;

    double _open;
    double _high;
    double _low;
    double _close;
    double _pchange;

    uint32_t _totalBuyQuantity;
    uint32_t _totalSellQuantity;
    uint32_t _volumeTradedToday;
    uint32_t _openInterest;

    ColorBitT _color;
};

using MarketWatchDataPtrT = std::shared_ptr<MarketWatchDataT>;

using ParameterValueT = struct ParameterValueT {
    bool        Check{ false };
    int         Integer{ 0 };
    float       Floating{ 0.0f };
    std::string Text;
};

using ParameterInfoT = struct ParameterInfoT {
    bool                SearchEnable{ false };
    DataType            Type;
    ParameterValueT     Parameter;
    ImGuiTextFilter     Filter;
    MarketWatchDataPtrT Self;
};

using GlobalParameterInfoT = struct GlobalParameterInfoT {
    bool           Update;
    std::string    Name;
    ParameterInfoT Info;
};

using OrderInfoT = struct OrderInfoT {
    uint32_t _portfolio;
    uint32_t Gateway;
    uint32_t _token;
    uint32_t _quantity;
    uint32_t FillQuantity;
    uint32_t _remaining;
    long     OrderNo;
    float    _price;
    float    _fillPrice;

    Lancelot::Side Side;
    OrderStatus    StatusValue;
    std::string    Contract;
    std::string    Time;
    std::string    Client;
    std::string    Message;
};

using NetBookColumnT = struct NetBookColumnT {
    uint32_t PF;
    uint32_t _buyQuantity;
    uint32_t _sellQuantity;
    int32_t  TotalQty;

    float LastLTP;
    float AverageBuyPrice;
    float AverageSellPrice;
    float NetInvestment;
    float TotalBuyPrice;
    float TotalSellPrice;

    float LastPNL;
    float PNL;
    float MTM;

    MarketWatchDataPtrT Self;
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

    MarketWatchDataPtrT _self;
    MarketWatchDataPtrT _future;
};
using GreeksPtrT = std::shared_ptr<GreeksT>;

using GreekBookColumnT = struct GreekBookColumnT {
    std::string Symbol;
    uint32_t    _buyQuantity;
    uint32_t    _sellQuantity;

    float TotalSellPrice;
    float TotalBuyPrice;
    float AverageSellPrice;
    float AverageBuyPrice;

    GreeksPtrT Greeks;
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
    bool     Close;
    uint32_t Inactive;
    uint32_t Active;
    uint32_t Apply;
    uint32_t Waiting;
    uint32_t Terminate;
};

using OrderFormInfoT = struct OrderFormInfoT {
    uint32_t            Gateway;
    double              Price;
    int                 Quantity;
    int                 LotSize;
    long                OrderNumber;
    int                 Type;
    Lancelot::Side      Side;
    OrderStatus         Status;
    std::string         Contract;
    std::string         Client;
    MarketWatchDataPtrT Self;
};

using SpotInfoT = struct SpotInfoT {
    float Value;
    float Change;
};

using ParameterInfoListT = std::map<std::string, ParameterInfoT>;

using StrategyRowT = struct StrategyRowT {
    bool               Changed;
    bool               Subscribed;
    bool               Selected;
    uint32_t           PF;
    StrategyStatus     Status;
    ParameterInfoListT ParameterInfoList;
};

using OptionChainItemT = struct OptionChainItemT {
    MarketWatchDataPtrT     _self;
    Lancelot::ResultSetPtrT _contract;
};

using OptionChainRowT = struct OptionChainRowT {
    OptionChainItemT _call;
    OptionChainItemT _put;
};

using ExcelContactItemT = struct ExcelContactItemT {
    size_t              Index;
    MarketWatchDataPtrT Self;
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
    int         PF;
    std::string Strategy;
    std::string Descriptions;
};

using SaveScannerItemT = struct SaveScannerItemT {
    bool        Applied;
    int64_t     UniqueID;
    std::string Name;
    std::string ExpandedEquation;
};

class Portfolio;
using PortfolioPtrT        = std::shared_ptr<Portfolio>;
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
using GlobalOrderInfoContainerT = std::unordered_map<int, OrderInfoPtrT>;

using PFWiseTradeContainerT      = std::map<std::pair<uint32_t, uint32_t>, NetBookColumnPtrT>;
using OptionChainContainerT      = std::map<float, OptionChainRowT>;
using ScannerInfoFromDatabaseT   = std::map<int, std::string>;
using ExcelContactItemContainerT = std::map<std::string, ExcelContactItemT>;
using PendingBookContainerT      = std::map<std::string, OrderInfoPtrT>;

using GlobalParamListT              = std::vector<GlobalParameterInfoT>;
using AllContractT                  = std::vector<std::string>;
using LiveContainerT                = std::deque<MarketWatchDataPtrT>;
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
using SubscribedT = std::set<uint32_t>;

template<typename Type, size_t Size = 30000>
using QueueT                = boost::lockfree::spsc_queue<Type, boost::lockfree::capacity<Size>>;
using PendingOrderUpdateT   = QueueT<std::pair<OrderInfoPtrT, bool>>;
using PendingTradeUpdateT   = QueueT<OrderInfoPtrT>;
using PendingTrackerUpdateT = QueueT<TradeTrackerItemT>;
using MarketEventQueueT     = QueueT<MarketWatchDataPtrT>;
using ScannerAddQueueT      = QueueT<StrategyRowPtrT>;
