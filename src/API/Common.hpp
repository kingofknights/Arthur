#pragma once

#include <array>
#include <cstdint>
#include <string>

enum SideType : int
{
	Side_BUY = 0,
	Side_SELL
};

enum OrderType : int
{
	OrderType_LIMIT = 0,
	OrderType_MARKET,
	OrderType_IOC,
	OrderType_SPREAD
};

enum StrategyStatus : int
{
	StrategyStatus_PENDING = 0,
	StrategyStatus_ACTIVE,
	StrategyStatus_APPLIED,
	StrategyStatus_INACTIVE,
	StrategyStatus_TERMINATED,
	StrategyStatus_WAITING,
	StrategyStatus_DISCONNECTED
};

enum RequestType : int
{
	RequestType_LOGIN = 0,
	// ORDER
	RequestType_NEW,
	RequestType_MODIFY,
	RequestType_CANCEL,
	// STRATEGY
	RequestType_APPLY,
	RequestType_SUBSCRIBE,
	RequestType_UNSUBSCRIBE,
	RequestType_SUBSCRIBE_APPLY,
};

enum ResponseType : int
{
	// ORDER
	ResponseType_PLACED = 0,
	ResponseType_NEW,
	ResponseType_REPLACED,
	ResponseType_CANCELLED,
	ResponseType_NEW_REJECT,
	ResponseType_REPLACE_REJECT,
	ResponseType_CANCEL_REJECT,
	ResponseType_PARTIAL_FILLED,
	ResponseType_FILLED,

	// STRATEGY
	ResponseType_PENDING,
	ResponseType_SUBCRIBED,
	ResponseType_APPLIED,
	ResponseType_UNSUBSCRIBED,
	ResponseType_TERMINATED,
	ResponseType_UPDATES,
	ResponseType_EXCHANGE_DISCONNECT,

	// TRACKER
	ResponseType_TRACKER,
};

enum OrderStatus : int
{
	OrderStatus_PLACED = 0,
	OrderStatus_NEW,
	OrderStatus_REPLACED,
	OrderStatus_CANCELLED,
	OrderStatus_NEW_REJECT,
	OrderStatus_REPLACE_REJECT,
	OrderStatus_CANCEL_REJECT,
	OrderStatus_PARTIAL_FILLED,
	OrderStatus_FILLED
};

#pragma pack(push, 1)

using RequestInPackT = struct RequestInPackT {
	short				  TotalSize;
	int					  Type;
	uint64_t			  UserIdentifier;
	int					  CompressedMsgLen;
	std::array<char, 512> Message;
};
#pragma pack(pop)
