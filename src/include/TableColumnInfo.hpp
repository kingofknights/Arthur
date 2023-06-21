#pragma once

#include "PreCompileHeader.hpp"
// clang-format off
inline constexpr const char* VisualThemeName[] = {
	"Material Light",
	"Material Dark",
	"One Light",
	"One Dark",
	"Arthur Light",
	"Arthur Dark",
	"Default Light",
	"Default Dark"
};

inline constexpr const char *StrategyStatusType[] = {
	"PENDING",
	"ACTIVE",
	"APPLIED",
	"INACTIVE",
	"TERMINATED",
	"WAITING",
	"DISCONNECTED",
};

inline constexpr const char *BookTableColumnName[] = {
	"PF",
	"Contract",
	"Price",
	"Quantity",
	"FillPrice",
	"FillQuantity",
	"Remaining",
	"Client",
	"Status",
	"Time",
	"GateWay",
	"OrderNumber",
	"Message"
};
inline constexpr const char *SymbolWiseTableColumnName[] = {
	"Contract",
	"AvgBid",
	"BuyQty",
	"SellQty",
	"AvgSell",
	"Total",
	"NetInvest",
	"MTM",
	"LTP",
	"PNL"
};
inline constexpr const char *PFWiseTableColumnName[] = {
	"PF",
	"Contract",
	"AvgBid",
	"BuyQty",
	"SellQty",
	"AvgSell",
	"Total",
	"NetInvest",
	"MTM",
	"LTP",
	"PNL"
};
inline constexpr const char *GreekBookColumnName[] = {
	"Symbol",
	"Delta",
	"Gamma",
	"Vega",
	"Theta",
	"MTM",
	"MarketRate",
	"Value"
};

inline constexpr const char *ColumnGeneratorTableName[] = {
	"NAME",
	"TYPE",
	"VALUE"
};

inline constexpr const char *ColumnGeneratorDataType[] = {
	"CLIENT",
	"COMBO",
	"CONTRACT",
	"FLOAT",
	"INT",
	"TEXT",
	"RADIO",
	"UPDATES"
};
inline constexpr const char *MarketWatchTableColumnName[] = {
	"Contract",
	"ATP",
	"LTP",
	"LTQ",
	"LTT",
	"TopBid",
	"TopAsk",
	"Open",
	"High",
	"Low",
	"Close",
	"LowDPR",
	"HighDPR",
	"TotalBuyQuantity",
	"TotalSellQuantity",
	"VolumeTradedToday",
	"OpenInterest"
};

inline constexpr const char *MarketWatchTableToolTipColumnName[] = {
	"Order",
	"Quantity",
	"Price",
	"Price",
	"Quantity",
	"Order"
};
inline constexpr const char *OptionChainTableColumnName[] = {
	"OI",
	"Volume",
	"Delta",
	"Gamma",
	"Vega",
	"Theta",
	"IV",
	"LTP",
	"Change",
	"Bid Qty",
	"Bid",
	"Ask",
	"Ask Qty",
	"Strike",
	"Bid Qty",
	"Bid",
	"Ask",
	"Ask Qty",
	"Change",
	"LTP",
	"IV",
	"Theta",
	"Vega",
	"Gamma",
	"Delta",
	"Volume",
	"OI"
};

inline constexpr const char *ExcelTableColumnName_1[] = {
	"LTP",
	"ATP",
	"Open",
	"High",
	"Low",
	"Close"
};
inline constexpr const char *ExcelTableColumnName_2[] = {
	"LowDPR",
	"HighDPR",
	"TBQ",
	"TSQ",
	"VTT",
	"OI"
};
inline constexpr const char *Configuration[] = {
	"Demo",
	"ExcelWindow",
	"Position",
	"Theme",
	"MarketLadder",
	"MarketWatch",
	"OpenOrders",
	"StrategyWorkspace",
	"TradeHistory",
	"OrderAllBook",
	"RejectBook",
	"OptionChain",
	"END"
};

inline constexpr const char *ScannerFunctionList[] = {
	"ATP",
	"PercentChange",
	"Open",
	"High",
	"Low",
	"Close",
	"HighDPR",
	"LowDPR",
	"TotalBuyQuantity",
	"TotalSellQuantity",
	"VolumeTradedToday",
	"OpenInterest",
	"LTP",
	"LTQ",
	"BidDepth",
	"AskDepth",
	"Expiry",
	"ExpiryWeek",
	"IsCall",
	"IsPut",
	"IsFuture"
};
inline constexpr const char *ScannerTableColumnName[] = {
	"Functions",
	"Variables"
};
inline constexpr const char *TradeTrackerColumnName[] = {
	"ID",
	"Name",
	"Descriptions"
};

inline constexpr const char *ScannerSavedTableColumnName[] = {
	"#",
	"Name",
	"Operations"
};
inline constexpr const char* OrderStatusInfoName[] = {
	"PLACED",
	"NEW",
	"REPLACED",
	"CANCELLED",
	"NEW_REJECT",
	"REPLACE_REJECT",
	"CANCEL_REJECT",
	"PARTIAL_FILLED",
	"FILLED",
};

inline constexpr const char* OrderTypeName[] = {
	"Limit",
	"Market",
	"IOC",
	"Spread"
};

// clang-format on
