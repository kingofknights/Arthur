#pragma once

#include <array>
#include <cstdint>
#include <string>

#define JSON_PARAMS		   "params"
#define JSON_ID			   "id"
#define JSON_TOKEN		   "token"
#define JSON_PRICE		   "price"
#define JSON_QUANTITY	   "quantity"
#define JSON_CLIENT		   "client"
#define JSON_SIDE		   "side"
#define JSON_ORDER_ID	   "order_id"
#define JSON_ARGUMENTS	   "arguments"
#define JSON_PF_NUMBER	   "pf"
#define JSON_UNIQUE_ID	   "unique_id"
#define JSON_STRATEGY_NAME "name"
#define JSON_ORDER_TYPE	   "type"
#define JSON_TIME		   "time"
#define JSON_FILL_QUANTITY "fill_quantity"
#define JSON_FILL_PRICE	   "fill_price"
#define JSON_REMAINING	   "remaining"
#define JSON_MESSAGE	   "message"

enum SideType
{
	Side_BUY = 0,
	Side_SELL
};

enum OrderType
{
	OrderType_LIMIT = 0,
	OrderType_MARKET,
	OrderType_IOC,
	OrderType_SPREAD
};

enum StrategyStatus
{
	StrategyStatus_PENDING = 0,
	StrategyStatus_ACTIVE,
	StrategyStatus_APPLIED,
	StrategyStatus_INACTIVE,
	StrategyStatus_TERMINATED,
	StrategyStatus_WAITING,
	StrategyStatus_DISCONNECTED
};

enum RequestType
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

enum ResponseType
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

enum OrderStatus
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
