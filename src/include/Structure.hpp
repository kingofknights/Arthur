//
// Created by VIKLOD on 22-01-2023.
//

#pragma once

//#define DISABLE_LOGGING
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

#include "imgui.h"

#pragma pack(push, 1)

enum DataType : int;
enum OrderStatus : int;
enum StrategyStatus : int;
enum OrderType : int;

constexpr int MARKET_WATCH_LADDER_COUNT = 5;
constexpr int STRATEGY_NAME_LENGTH		= 50;

using PricePointsT = struct PricePointsT {
	float	 Price;
	uint32_t Quantity;
	uint32_t Order;
};

using ColorBitT = struct ColorBitT {
	uint8_t TopBid : 1;
	uint8_t TopAsk : 1;
	uint8_t LTP : 1;
	uint8_t ATP : 1;
	uint8_t Dummy : 4;
};

using MarketWatchDataT = struct MarketWatchDataT {
	std::array<PricePointsT, MARKET_WATCH_LADDER_COUNT> Bid;
	std::array<PricePointsT, MARKET_WATCH_LADDER_COUNT> Ask;
	std::array<char, STRATEGY_NAME_LENGTH>				LastTradeTime;
	std::array<char, STRATEGY_NAME_LENGTH>				Description;

	uint32_t Token;
	uint32_t LastTradeQuantity;
	float	 AverageTradePrice;
	float	 LastTradePrice;
	float	 LowDPR;
	float	 HighDPR;

	float OpenPrice;
	float HighPrice;
	float LowPrice;
	float ClosePrice;
	float PercentageChange;

	uint32_t TotalBuyQuantity;
	uint32_t TotalSellQuantity;
	uint32_t VolumeTradedToday;
	uint32_t OpenInterest;

	ColorBitT Color;
};

using MarketWatchDataPtrT = std::shared_ptr<MarketWatchDataT>;

using ParameterValueT = struct ParameterValueT {
	bool		Check{false};
	int			Integer{0};
	float		Floating{0.0f};
	std::string Text;
};

using ParameterInfoT = struct ParameterInfoT {
	bool				SearchEnable{false};
	DataType			Type;
	ParameterValueT		Parameter;
	ImGuiTextFilter		Filter;
	MarketWatchDataPtrT Self;
};

using GlobalParameterInfoT = struct GlobalParameterInfoT {
	bool		   Update;
	std::string	   Name;
	ParameterInfoT Info;
};

using OrderInfoT = struct OrderInfoT {
	uint32_t PF;
	uint32_t Gateway;
	uint32_t Token;
	uint32_t Quantity;
	uint32_t FillQuantity;
	uint32_t Remaining;
	long	 OrderNo;
	float	 Price;
	float	 FillPrice;

	Lancelot::Side Side;
	OrderStatus	   StatusValue;
	std::string	   Contract;
	std::string	   Time;
	std::string	   Client;
	std::string	   Message;
};

using NetBookColumnT = struct NetBookColumnT {
	uint32_t PF;
	uint32_t BuyQuantity;
	uint32_t SellQuantity;
	int32_t	 TotalQty;

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
	bool	 IsCall;
	bool	 IsFuture;
	uint32_t Expiry;
	float	 StrikePrice;
	float	 IV;
	float	 Delta;
	float	 Gamma;
	float	 Vega;
	float	 Theta;

	MarketWatchDataPtrT Self;
	MarketWatchDataPtrT Future;
};
using GreeksPtrT = std::shared_ptr<GreeksT>;

using GreekBookColumnT = struct GreekBookColumnT {
	std::string Symbol;
	uint32_t	BuyQuantity;
	uint32_t	SellQuantity;

	float TotalSellPrice;
	float TotalBuyPrice;
	float AverageSellPrice;
	float AverageBuyPrice;

	GreeksPtrT Greeks;
};

using DValueT = struct DValueT {
	float		Delta;
	float		Gamma;
	float		Vega;
	float		Theta;
	float		MTM;
	float		MarketRate;
	float		Value;
	std::string Symbol;
};

using PortfolioStatusT = struct PortfolioStatusT {
	bool	 Close;
	uint32_t Inactive;
	uint32_t Active;
	uint32_t Apply;
	uint32_t Waiting;
	uint32_t Terminate;
};

using OrderFormInfoT = struct OrderFormInfoT {
	uint32_t			Gateway;
	double				Price;
	int					Quantity;
	int					LotSize;
	long				OrderNumber;
	int					Type;
	Lancelot::Side		Side;
	OrderStatus			Status;
	std::string			Contract;
	std::string			Client;
	MarketWatchDataPtrT Self;
};

using SpotInfoT = struct SpotInfoT {
	float Value;
	float Change;
};

using ParameterInfoListT = std::map<std::string, ParameterInfoT>;

using StrategyRowT = struct StrategyRowT {
	bool			   Changed;
	bool			   Subscribed;
	bool			   Selected;
	uint32_t		   PF;
	StrategyStatus	   Status;
	ParameterInfoListT ParameterInfoList;
};

using OptionChainItemT = struct OptionChainItemT {
	MarketWatchDataPtrT		Self;
	Lancelot::ResultSetPtrT ResultSet;
};

using OptionChainRowT = struct OptionChainRowT {
	OptionChainItemT Call;
	OptionChainItemT Put;
};

using ExcelContactItemT = struct ExcelContactItemT {
	size_t				Index;
	MarketWatchDataPtrT Self;
};

using ScannerFunctionInfoT = struct ScannerFunctionInfoT {
	bool		Selected;
	char		Variable;
	std::string Name;
};

using ClientInfoT = struct ClientInfoT {
	Lancelot::Exchange Exchange;
	std::string		   ClientCode;
};

using TradeTrackerItemT = struct TradeTrackerItemT {
	int			PF;
	std::string Strategy;
	std::string Descriptions;
};

using SaveScannerItemT = struct SaveScannerItemT {
	bool		Applied;
	int64_t		UniqueID;
	std::string Name;
	std::string ExpandedEquation;
};

class Portfolio;
using PortfolioPtrT		   = std::shared_ptr<Portfolio>;
using ScannerResultOutputT = struct ScannerResultOutputT {
	PortfolioPtrT	   PortfolioPtr;
	ParameterInfoListT ParameterInfoList;
};
#pragma pack(pop)

struct Singleton {
	Singleton()								= default;
	Singleton(const Singleton &)			= delete;
	Singleton(Singleton &&)					= delete;
	Singleton &operator=(const Singleton &) = delete;
	Singleton &operator=(Singleton &&)		= delete;
};

class ExcelAutomation;
using ExcelAutomationPtrT = std::unique_ptr<ExcelAutomation>;
using StrategyRowPtrT	  = std::shared_ptr<StrategyRowT>;
using WeakStrategyRowPtrT = std::weak_ptr<StrategyRowT>;
using NetBookColumnPtrT	  = std::shared_ptr<NetBookColumnT>;
using GreekBookColumnPtrT = std::shared_ptr<GreekBookColumnT>;
using OrderInfoPtrT		  = std::shared_ptr<OrderInfoT>;

using MarketWatchDatContainerT	= std::unordered_map<uint32_t, MarketWatchDataPtrT>;
using GlobalStrategyListT		= std::unordered_map<uint32_t, WeakStrategyRowPtrT>;
using GreekBookContainerT		= std::unordered_map<uint32_t, GreekBookColumnPtrT>;
using SymbolWiseTradeContainerT = std::unordered_map<uint32_t, NetBookColumnPtrT>;
using GlobalOrderInfoContainerT = std::unordered_map<int, OrderInfoPtrT>;

using PFWiseTradeContainerT		 = std::map<std::pair<uint32_t, uint32_t>, NetBookColumnPtrT>;
using OptionChainContainerT		 = std::map<float, OptionChainRowT>;
using ScannerInfoFromDatabaseT	 = std::map<int, std::string>;
using ExcelContactItemContainerT = std::map<std::string, ExcelContactItemT>;
using PendingBookContainerT		 = std::map<std::string, OrderInfoPtrT>;

using GlobalParamListT				= std::vector<GlobalParameterInfoT>;
using AllContractT					= std::vector<std::string>;
using LiveContainerT				= std::deque<MarketWatchDataPtrT>;
using StrategyNameListT				= AllContractT;
using ClientCodeListT				= std::vector<ClientInfoT>;
using StrategyListT					= std::deque<StrategyRowPtrT>;
using ScannerFunctionListContainerT = std::vector<ScannerFunctionInfoT>;
using TradeTrackerContainerT		= std::deque<TradeTrackerItemT>;
using ScannerSaveContainerT			= std::vector<SaveScannerItemT>;
using SymbolWiseTradeContainerVecT	= std::deque<std::pair<int, NetBookColumnPtrT>>;
using PFWiseTradeContainerVecT		= std::deque<std::pair<std::pair<int, int>, NetBookColumnPtrT>>;
using BookOrderListT				= std::deque<OrderInfoPtrT>;

using GlobalPortfolioScannerContainerT = std::unordered_map<int64_t, ScannerResultOutputT>;

using GreeksListT = std::list<GreeksPtrT>;
using SubscribedT = std::set<uint32_t>;

template <typename Type, size_t Size = 30000>
using QueueT				= boost::lockfree::spsc_queue<Type, boost::lockfree::capacity<Size>>;
using PendingOrderUpdateT	= QueueT<std::pair<OrderInfoPtrT, bool>>;
using PendingTradeUpdateT	= QueueT<OrderInfoPtrT>;
using PendingTrackerUpdateT = QueueT<TradeTrackerItemT>;
using MarketEventQueueT		= QueueT<MarketWatchDataPtrT>;
using ScannerAddQueueT		= QueueT<StrategyRowPtrT>;
