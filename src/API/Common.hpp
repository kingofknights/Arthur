#pragma once

#include <array>
#include <cstdint>
#include <string>

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
