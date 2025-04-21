//
// Created by VIKLOD on 15-06-2023.
//

#include "ContractInfo.hpp"

#include "ContractFetcher.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "StoreProcedures.hpp"
#include "Structure.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace Lancelot {

    using ResultSetContainerT   = std::unordered_map<int32_t, ResultSetPtrT>;
    using NameToTokenContainerT = std::unordered_map<std::string, int32_t>;

    namespace details {
        static ResultSetContainerT   ResultSetContainer;
        static NameToTokenContainerT NameToTokenContainer;
        static ContractFetcher*      contractFetcher = nullptr;
    }  // namespace details

#define GET_RESULT_SET(TYPE, FUNCTION, FIELD)                     \
    auto ContractInfo::Get##FUNCTION(int32_t token_)->TYPE {      \
        auto iterator = details::ResultSetContainer.find(token_); \
        if (iterator != details::ResultSetContainer.end()) {      \
            return iterator->second->FIELD;                       \
        }                                                         \
        return {};                                                \
    }

    void LoadResultSetTable(const TableWithColumnNameT& table_, const ResultSetLoadingCallbackT& callback_) {
        for (const auto& row : table_) {
            auto* resultSetPtr = new ResultSetT;

            resultSetPtr->_token       = std::stoi(row.at("Token"));
            resultSetPtr->_futureToken = std::stoi(row.at("Future"));
            resultSetPtr->_expiryDate  = std::stoi(row.at("ExpiryDate"));
            resultSetPtr->_lotSize     = std::stoi(row.at("LotSize"));
            resultSetPtr->_tickSize    = std::stoi(row.at("TickSize"));
            resultSetPtr->_divisor     = std::stoi(row.at("Divisor"));
            resultSetPtr->_option      = Lancelot::ContractInfo::GetOptionType(row.at("OptionType"));
            resultSetPtr->_instType    = Lancelot::ContractInfo::GetInstrumentType(row.at("InstType"));
            resultSetPtr->_exchange    = Lancelot::ContractInfo::GetExchange(row.at("Exchange"));
            resultSetPtr->_strikePrice = std::stof(row.at("StrikePrice"));
            resultSetPtr->_symbol      = row.at("Symbol");
            resultSetPtr->_segment     = row.at("Segment");
            resultSetPtr->_name        = row.at("Name");

            float close   = std::stof(row.at("Close")) / resultSetPtr->_divisor;
            float lowDPR  = std::stof(row.at("LowDPR")) / resultSetPtr->_divisor;
            float highDPR = std::stof(row.at("HighDPR")) / resultSetPtr->_divisor;

            {
                std::stringstream ss;
                ss << (resultSetPtr->_strikePrice < 0 ? "FUT" : "OPT");
                ss << ' ' << resultSetPtr->_symbol.data();
                if (resultSetPtr->_strikePrice > 0) ss << ' ' << FORMAT("{:.2f}", resultSetPtr->_strikePrice / (resultSetPtr->_divisor * 1.0F)) << ' ' << (resultSetPtr->_option == Lancelot::OptionType_CALL ? "CE" : "PE");
                ss << ' ' << FORMAT("{:%d%b}", fmt::localtime(resultSetPtr->_expiryDate));
                auto description           = boost::to_upper_copy(ss.str());
                resultSetPtr->_description = description;
            }
            resultSetPtr->_strikePrice /= resultSetPtr->_divisor;
            details::ResultSetContainer.emplace(resultSetPtr->_token, resultSetPtr);
            details::NameToTokenContainer.emplace(resultSetPtr->_description, resultSetPtr->_token);
            if (callback_) {
                callback_(resultSetPtr, close, lowDPR, highDPR);
            }
        }
    }

    void ContractInfo::Initialize(const std::string& name_, const ResultSetLoadingCallbackT& callback_) {
        details::contractFetcher = new ContractFetcher(name_);
        const auto table         = details::contractFetcher->GetResultWithColumnName(GetResultSet_);
        LoadResultSetTable(table, callback_);
    }

    auto ContractInfo::GetResultSet(int32_t token_) -> ResultSetPtrT {
        auto iterator = details::ResultSetContainer.find(token_);
        if (iterator != details::ResultSetContainer.end()) return iterator->second;
        return nullptr;
    }

    auto ContractInfo::GetToken(const std::string& name_) -> int32_t {
        auto iterator = details::NameToTokenContainer.find(name_);
        if (iterator != details::NameToTokenContainer.cend()) return iterator->second;
        return 0;
    }

    GET_RESULT_SET(int32_t, ExpiryDate, _expiryDate)
    GET_RESULT_SET(int32_t, Future, _futureToken)
    GET_RESULT_SET(int32_t, LotSize, _lotSize)
    GET_RESULT_SET(int32_t, TickSize, _tickSize)
    GET_RESULT_SET(int32_t, Divisor, _divisor)
    GET_RESULT_SET(float, StrikePrice, _strikePrice)
    GET_RESULT_SET(Instrument, InstType, _instType)
    GET_RESULT_SET(OptionType, Option, _option)
    GET_RESULT_SET(Exchange, Exchange, _exchange)
    GET_RESULT_SET(std::string, Symbol, _symbol)
    GET_RESULT_SET(std::string, Segment, _segment)
    GET_RESULT_SET(std::string, Name, _name)
    GET_RESULT_SET(std::string, Description, _description)

#undef GET_RESULT_SET

    auto ContractInfo::GetOppositeToken(int32_t token_) -> int32_t {
        std::string       name    = GetDescription(token_);
        bool              isCall  = IsCall(token_);
        const std::string callPut = isCall ? "CE" : "PE";
        const std::string reverse = isCall ? "PE" : "CE";
        size_t            pos     = name.find(callPut);
        if (pos != std::string::npos) {
            name.replace(pos, callPut.length(), reverse);
            return GetToken(name);
        }
        return 0;
    }

    auto ContractInfo::IsOption(int32_t token_) -> bool { return GetInstType(token_) == Instrument_OPTION; }

    auto ContractInfo::IsEquity(int32_t token_) -> bool { return GetInstType(token_) == Instrument_EQUITY; }

    auto ContractInfo::IsCall(int32_t token_) -> bool { return GetOption(token_) == OptionType_CALL; }

    auto ContractInfo::IsPut(int32_t token_) -> bool { return GetOption(token_) == OptionType_PUT; }

    auto ContractInfo::IsFuture(int32_t token_) -> bool { return GetInstType(token_) == Instrument_FUTURE; }

    void ContractInfo::ExecuteQuery(const std::string& query_) { details::contractFetcher->ExecuteQuery(query_); }

    auto ContractInfo::GetResultWithIndex(const std::string& query_) -> TableWithColumnIndexT { return details::contractFetcher->GetResultWithColumnIndex(query_); }

    auto ContractInfo::GetResultWithName(const std::string& query_) -> TableWithColumnNameT { return details::contractFetcher->GetResultWithColumnName(query_); }

    auto ContractInfo::GetOptionType(const std::string& option_) -> OptionType {
        if (option_ == "CE") return OptionType_CALL;
        if (option_ == "PE") return OptionType_PUT;
        return OptionType_NONE;
    }

    auto ContractInfo::GetInstrumentType(const std::string& type_) -> Instrument {
        if (type_.starts_with("FUT")) return Instrument_FUTURE;
        if (type_.starts_with("OPT")) return Instrument_OPTION;
        return Instrument_EQUITY;
    }

    auto ContractInfo::GetExchange(const std::string& exchange_) -> Exchange {
        if (exchange_ == "NSE_FO") return Exchange_NSE_FUTURE;
        if (exchange_ == "NSE_EQ") return Exchange_NSE_EQUITY;
        if (exchange_ == "NSE_CD") return Exchange_NSE_CURRENCY;
        if (exchange_ == "BSE_FO") return Exchange_BSE_FUTURE;
        if (exchange_ == "BSE_CD") return Exchange_BSE_CURRENCY;
        return Exchange_END;
    }

}  // namespace Lancelot
