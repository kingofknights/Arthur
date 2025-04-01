//
// Created by kingofknights on 7/9/23.
//

#include "Lancelot/Enums.hpp"

namespace Lancelot {

    auto ToString(Exchange exchangeCode_) -> std::string {
        switch (exchangeCode_) {
            case Exchange_NSE_FUTURE:
                return "NSE_FUTURE";
            case Exchange_NSE_EQUITY:
                return "NSE_EQUITY";
            case Exchange_NSE_CURRENCY:
                return "NSE_CURRENCY";
            case Exchange_BSE_FUTURE:
                return "BSE_FUTURE";
            case Exchange_BSE_CURRENCY:
                return "BSE_CURRENCY";
            case Exchange_END:
                return "Exchange_END";
        }
        return __FUNCTION__;
    }

    auto ToString(Instrument instrument_) -> std::string {
        switch (instrument_) {
            case Instrument_FUTURE:
                return "Instrument_FUTURE";
            case Instrument_OPTION:
                return "Instrument_OPTION";
            case Instrument_EQUITY:
                return "Instrument_EQUITY";
            case Instrument_OTHER:
                return "Instrument_OTHER";
        }
        return __FUNCTION__;
    }

    auto ToString(OptionType optionType_) -> std::string {
        switch (optionType_) {
            case OptionType_NONE:
                return "OptionType_NONE";
            case OptionType_CALL:
                return "OptionType_CALL";
            case OptionType_PUT:
                return "OptionType_PUT";
        }
        return __FUNCTION__;
    }

    auto ToString(Side side_) -> std::string {
        switch (side_) {
            case Side_BUY:
                return "Side_BUY";
            case Side_SELL:
                return "Side_SELL";
        }
        return __FUNCTION__;
    }

    auto ToString(RequestType requestType_) -> std::string {
        switch (requestType_) {
            case RequestType_LOGIN:
                return "RequestType_LOGIN";
            case RequestType_NEW:
                return "RequestType_NEW";
            case RequestType_MODIFY:
                return "RequestType_MODIFY";
            case RequestType_CANCEL:
                return "RequestType_CANCEL";
            case RequestType_APPLY:
                return "RequestType_APPLY";
            case RequestType_SUBSCRIBE:
                return "RequestType_SUBSCRIBE";
            case RequestType_UNSUBSCRIBE:
                return "RequestType_UNSUBSCRIBE";
            case RequestType_SUBSCRIBE_APPLY:
                return "RequestType_SUBSCRIBE_APPLY";
        }
        return __FUNCTION__;
    }

    auto ToString(ResponseType responseType_) -> std::string {
        switch (responseType_) {
            case ResponseType_PLACED:
                return "ResponseType_PLACED";
            case ResponseType_NEW:
                return "ResponseType_NEW";
            case ResponseType_REPLACED:
                return "ResponseType_REPLACED";
            case ResponseType_CANCELLED:
                return "ResponseType_CANCELLED";
            case ResponseType_NEW_REJECT:
                return "ResponseType_NEW_REJECT";
            case ResponseType_REPLACE_REJECT:
                return "ResponseType_REPLACE_REJECT";
            case ResponseType_CANCEL_REJECT:
                return "ResponseType_CANCEL_REJECT";
            case ResponseType_PARTIAL_FILLED:
                return "ResponseType_PARTIAL_FILLED";
            case ResponseType_FILLED:
                return "ResponseType_FILLED";
            case ResponseType_PENDING:
                return "ResponseType_PENDING";
            case ResponseType_SUBCRIBED:
                return "ResponseType_SUBCRIBED";
            case ResponseType_APPLIED:
                return "ResponseType_APPLIED";
            case ResponseType_UNSUBSCRIBED:
                return "ResponseType_UNSUBSCRIBED";
            case ResponseType_TERMINATED:
                return "ResponseType_TERMINATED";
            case ResponseType_UPDATES:
                return "ResponseType_UPDATES";
            case ResponseType_EXCHANGE_DISCONNECT:
                return "ResponseType_EXCHANGE_DISCONNECT";
            case ResponseType_TRACKER:
                return "ResponseType_TRACKER";
        }
        return __FUNCTION__;
    }

}  // namespace Lancelot
