#pragma once

enum OrderType : int {
    OrderType_LIMIT = 1,
    OrderType_MARKET,
    OrderType_IOC = 3,
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
    OrderStatus_NONE,
    OrderStatus_PENDING,
    OrderStatus_NEW,
    OrderStatus_REPLACED,
    OrderStatus_CANCELLED,
    OrderStatus_PARTIAL_FILLED,
    OrderStatus_FILLED,
    OrderStatus_NEW_REJECT,
    OrderStatus_REPLACE_REJECT,
    OrderStatus_CANCEL_REJECT,
};
