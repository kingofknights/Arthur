#ifndef LANCELOT_INCLUDE_STRUCTURE_HPP
#define LANCELOT_INCLUDE_STRUCTURE_HPP
#include <cstdint>
#pragma once

#include "Lancelot/Enums.hpp"

namespace Lancelot {

    using ResultSetT = struct ResultSetT {
        uint32_t   _token       = 0;
        uint32_t   _futureToken = 0;
        uint32_t   _expiryDate  = 0;
        uint32_t   _lotMultiple = 0;
        uint32_t   _lotSize     = 0;
        uint32_t   _tickSize    = 0;
        uint32_t   _divisor     = 0;
        Instrument _instType    = Instrument_OTHER;
        OptionType _option      = OptionType_NONE;
        Exchange   _exchange    = Exchange_END;
        float      _strikePrice = 0;

        std::string _symbol;
        std::string _segment;
        std::string _name;
        std::string _description;
    };

#pragma pack(push, 1)
    struct Header {
        int16_t _type;
        int16_t _length;
    };
    struct UserPortfolio {
        int16_t _user;
        int16_t _portfolio;
    };

    struct HedgeOrderResponse {
        Header        _header;
        UserPortfolio _user;
        int32_t       _clientOrderNumber;
        int32_t       _syntheticNumber;
        uint64_t      _exchangeOrderNumber;
        uint32_t      _token;
        uint32_t      _price;
        uint32_t      _quantity;
        int16_t       _side;
        int32_t       _orderStatus;
        uint64_t      _timestamp;
        int           _errorCode;
    };

    struct TradeConfirmation {
        Header   _header;
        int32_t  _userId;
        int32_t  _sequenceNumber;
        int16_t  _portfolio;
        uint32_t _token;
        int32_t  _clientOrderNumber;
        uint64_t _exchangeOrderNumber;
        int32_t  _syntheticNumber;
        int16_t  _side;
        uint32_t _fillPrice;
        uint32_t _fillQuantity;
        uint32_t _price;
        uint32_t _quantity;
        uint32_t _remainingQuantity;
        uint64_t _timestamp;
        int32_t  _userSpread;
        int32_t  _fillNumber;
    };
    struct ManualOrder {
        Header        _header;
        UserPortfolio _user;

        uint32_t _token;
        uint32_t _price;
        uint32_t _quantity;
        uint32_t _triggerPrice;
        int16_t  _side;
        int32_t  _orderSequence;
        int16_t  _orderType;
        uint64_t _nnf;
    };

    struct ModifyOrder {
        Header        _header;
        UserPortfolio _user;

        uint32_t _token;
        int32_t  _orderSequence;
        uint32_t _price;
        uint32_t _quantity;
        uint32_t _triggerPrice;
    };

    struct CancelOrder {
        Header        _header;
        UserPortfolio _user;

        uint32_t _token;
        int32_t  _orderSequence;
    };

    struct KillSwitch {
        Header        _header;
        UserPortfolio _user;
    };

#pragma pack(pop)
}  // namespace Lancelot

#endif  // LANCELOT_INCLUDE_STRUCTURE_HPP
