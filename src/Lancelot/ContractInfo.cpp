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
#include <unordered_map>

namespace Lancelot {

    constexpr static int TenYearsInSeconds = 315513000;
    using ResultSetContainerT              = std::unordered_map<uint32_t, ResultSetPtrT>;
    using NameToTokenContainerT            = std::unordered_map<std::string, uint32_t>;

    namespace details {
        static ResultSetContainerT   ResultSetContainer;
        static NameToTokenContainerT NameToTokenContainer;
        static ContractFetcher*      contractFetcher = nullptr;
    }  // namespace details

#define GET_RESULT_SET(TYPE, FUNCTION, FIELD)                     \
    auto ContractInfo::Get##FUNCTION(uint32_t token_)->TYPE {     \
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
            resultSetPtr->_expiryDate  = std::stoi(row.at("ExpiryDate"));
            resultSetPtr->_lotMultiple = std::stoi(row.at("LotMultiple"));
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
                if (resultSetPtr->_strikePrice > 0) {
                    ss << ' ' << (resultSetPtr->_strikePrice) << ' ' << (resultSetPtr->_option == Lancelot::OptionType_CALL ? "CE" : "PE");
                }
                ss << ' ' << ToHuman(resultSetPtr->_expiryDate + TenYearsInSeconds);
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
        std::fstream file(name_, std::ios::in);
        if (not file.is_open()) {
            LOG(INFO, "unable to open contract file {}", name_);
            return;
        }
        // TableWithColumnIndexT table;

        while (not file.eof()) {
            std::string data;
            std::getline(file, data);

            if (data.empty() or std::isalpha(data[0]) != 0) {
                continue;
            }

            RowWithColumnIndexT result;
            boost::split(result, data, boost::is_any_of(" ,"));

            auto* resultSetPtr = new ResultSetT;

            resultSetPtr->_token       = boost::lexical_cast<uint32_t>(result[TOKEN]);
            resultSetPtr->_futureToken = boost::lexical_cast<uint32_t>(result[FUTURE_TOKEN]);
            resultSetPtr->_expiryDate  = boost::lexical_cast<uint32_t>(result[EXPIRY]);
            resultSetPtr->_lotMultiple = boost::lexical_cast<uint32_t>(result[BLQ]);
            resultSetPtr->_lotSize     = boost::lexical_cast<uint32_t>(result[LOT_SIZE]);
            resultSetPtr->_tickSize    = boost::lexical_cast<uint32_t>(result[TICK_SIZE]);
            resultSetPtr->_divisor     = 100;
            resultSetPtr->_instType    = GetInstrumentType(result[INSTRUMENT_TYPE]);
            resultSetPtr->_option      = GetOptionType(result[OPTION_TYPE]);
            resultSetPtr->_exchange    = Exchange_NSE_FUTURE;
            resultSetPtr->_strikePrice = boost::lexical_cast<float>(result[STRIKE]);
            resultSetPtr->_symbol      = result[SYMBOL];
            resultSetPtr->_segment     = "F&O";

            resultSetPtr->_name = result[DESCRIPTION];

            {
                std::stringstream ss;
                ss << (resultSetPtr->_strikePrice < 0 ? "FUT" : "OPT");
                ss << ' ' << resultSetPtr->_symbol.data();
                if (resultSetPtr->_strikePrice > 0) ss << ' ' << (resultSetPtr->_strikePrice) << ' ' << (resultSetPtr->_option == Lancelot::OptionType_CALL ? "CE" : "PE");
                ss << ' ' << ToHuman(resultSetPtr->_expiryDate + TenYearsInSeconds, "%d%b");
                auto description           = boost::to_upper_copy(ss.str());
                resultSetPtr->_description = description;
            }

            auto low  = boost::lexical_cast<float>(result[LOW_DPR]);
            auto high = boost::lexical_cast<float>(result[HIGH_DPR]);
            details::ResultSetContainer.emplace(resultSetPtr->_token, resultSetPtr);
            details::NameToTokenContainer.emplace(resultSetPtr->_description, resultSetPtr->_token);
            callback_(resultSetPtr, low, low, high);
            // table.push_back(result);
        }
        details::contractFetcher = new ContractFetcher("ResultSet.db3");
        // details::contractFetcher->Insert(table);
    }

    auto ContractInfo::GetResultSet(uint32_t token_) -> ResultSetPtrT {
        auto iterator = details::ResultSetContainer.find(token_);
        if (iterator != details::ResultSetContainer.end()) return iterator->second;
        return nullptr;
    }

    auto ContractInfo::GetToken(const std::string& name_) -> uint32_t {
        auto iterator = details::NameToTokenContainer.find(name_);
        if (iterator != details::NameToTokenContainer.cend()) return iterator->second;
        return 0;
    }

    GET_RESULT_SET(uint32_t, ExpiryDate, _expiryDate)
    GET_RESULT_SET(uint32_t, Future, _futureToken)
    GET_RESULT_SET(uint32_t, LotMultiple, _lotMultiple)
    GET_RESULT_SET(uint32_t, LotSize, _lotSize)
    GET_RESULT_SET(uint32_t, TickSize, _tickSize)
    GET_RESULT_SET(uint32_t, Divisor, _divisor)
    GET_RESULT_SET(float, StrikePrice, _strikePrice)
    GET_RESULT_SET(Instrument, InstType, _instType)
    GET_RESULT_SET(OptionType, Option, _option)
    GET_RESULT_SET(Exchange, Exchange, _exchange)
    GET_RESULT_SET(std::string, Symbol, _symbol)
    GET_RESULT_SET(std::string, Segment, _segment)
    GET_RESULT_SET(std::string, Name, _name)
    GET_RESULT_SET(std::string, Description, _description)

#undef GET_RESULT_SET

    auto ContractInfo::GetOppositeToken(uint32_t token_) -> uint32_t {
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

    auto ContractInfo::IsOption(uint32_t token_) -> bool { return GetInstType(token_) == Instrument_OPTION; }

    auto ContractInfo::IsEquity(uint32_t token_) -> bool { return GetInstType(token_) == Instrument_EQUITY; }

    auto ContractInfo::IsCall(uint32_t token_) -> bool { return GetOption(token_) == OptionType_CALL; }

    auto ContractInfo::IsPut(uint32_t token_) -> bool { return GetOption(token_) == OptionType_PUT; }

    auto ContractInfo::IsFuture(uint32_t token_) -> bool { return GetInstType(token_) == Instrument_FUTURE; }

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
