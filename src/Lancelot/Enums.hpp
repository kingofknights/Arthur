//
// Created by kingofknights on 7/8/23.
//

#ifndef LANCELOT_INCLUDE_ENUMS_HPP
#define LANCELOT_INCLUDE_ENUMS_HPP
#pragma once

#include <string>

inline static constexpr char JSON_PARAMS[]        = "params";
inline static constexpr char JSON_ID[]            = "uniqueId";
inline static constexpr char JSON_TOKEN[]         = "token";
inline static constexpr char JSON_PRICE[]         = "price";
inline static constexpr char JSON_QUANTITY[]      = "quantity";
inline static constexpr char JSON_CLIENT[]        = "client";
inline static constexpr char JSON_SIDE[]          = "side";
inline static constexpr char JSON_ORDER_ID[]      = "order_id";
inline static constexpr char JSON_ARGUMENTS[]     = "arguments";
inline static constexpr char JSON_PF_NUMBER[]     = "pf";
inline static constexpr char JSON_UNIQUE_ID[]     = "unique_id";
inline static constexpr char JSON_STRATEGY_NAME[] = "name";
inline static constexpr char JSON_ORDER_TYPE[]    = "type";
inline static constexpr char JSON_TIME[]          = "time";
inline static constexpr char JSON_FILL_QUANTITY[] = "fill_quantity";
inline static constexpr char JSON_FILL_PRICE[]    = "fill_price";
inline static constexpr char JSON_REMAINING[]     = "remaining";
inline static constexpr char JSON_MESSAGE[]       = "message";

namespace Lancelot {

    enum Exchange : uint8_t {
        Exchange_NSE_FUTURE = 0,
        Exchange_NSE_EQUITY,
        Exchange_NSE_CURRENCY,
        Exchange_BSE_FUTURE,
        Exchange_BSE_CURRENCY,
        Exchange_END
    };

    enum Instrument : uint8_t {
        Instrument_FUTURE = 0,
        Instrument_OPTION,
        Instrument_EQUITY,
        Instrument_OTHER
    };

    enum OptionType : uint8_t {
        OptionType_NONE = 0,
        OptionType_CALL,
        OptionType_PUT,
    };

    enum Side : uint8_t {
        Side_BUY = 1,
        Side_SELL
    };

    enum RequestType : uint8_t {
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

    enum ResponseType : uint8_t {
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

    auto ToString(Exchange exchangeCode_) -> std::string;
    auto ToString(Instrument instrument_) -> std::string;
    auto ToString(OptionType optionType_) -> std::string;
    auto ToString(Side side_) -> std::string;
    auto ToString(RequestType requestType_) -> std::string;
    auto ToString(ResponseType responseType_) -> std::string;

}  // namespace Lancelot
#endif  // LANCELOT_INCLUDE_ENUMS_HPP
