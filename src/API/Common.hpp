#pragma once

#include <array>
#include <cstdint>
#include <string>

enum OrderType : int {
    OrderType_LIMIT = 0,
    OrderType_MARKET,
    OrderType_IOC,
    OrderType_SPREAD
};

enum StrategyStatus : int {
    StrategyStatus_PENDING = 0,
    StrategyStatus_ACTIVE,
    StrategyStatus_APPLIED,
    StrategyStatus_INACTIVE,
    StrategyStatus_TERMINATED,
    StrategyStatus_WAITING,
    StrategyStatus_DISCONNECTED
};

enum OrderStatus : int {
    OrderStatus_PLACED         = 10,
    OrderStatus_NEW            = 20,
    OrderStatus_REPLACED       = 21,
    OrderStatus_CANCELLED      = 22,
    OrderStatus_NEW_REJECT     = 30,
    OrderStatus_REPLACE_REJECT = 31,
    OrderStatus_CANCEL_REJECT  = 32,
    OrderStatus_PARTIAL_FILLED = 40,
    OrderStatus_FILLED         = 41
};
