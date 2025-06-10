//
// Created by VIKLOD on 15-06-2023.
//

#ifndef LANCELOT_INCLUDE_CONTRACT_INFO_HPP
#define LANCELOT_INCLUDE_CONTRACT_INFO_HPP
#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Lancelot {
    enum Exchange : uint8_t;
    enum Instrument : uint8_t;
    enum OptionType : uint8_t;

    struct ResultSetT;
    using ResultSetPtrT             = const ResultSetT*;
    using RowWithColumnNameT        = std::unordered_map<std::string, std::string>;
    using RowWithColumnIndexT       = std::vector<std::string>;
    using TableWithColumnNameT      = std::vector<RowWithColumnNameT>;
    using TableWithColumnIndexT     = std::vector<RowWithColumnIndexT>;
    using ResultSetLoadingCallbackT = std::function<void(ResultSetPtrT, float, float, float)>;
    using ResultSetContainerT       = std::unordered_map<int32_t, ResultSetPtrT>;

    class ContractInfo {
      public:
        static void Initialize(const std::string& name_, const ResultSetLoadingCallbackT& callback_);
        static auto GetResultSet(int32_t token_) -> ResultSetPtrT;
        static auto GetCompeleteContract() noexcept -> ResultSetContainerT&;

        static auto GetToken(const std::string& name_) -> int32_t;
        static auto GetExpiryDate(int32_t token_) -> int32_t;
        static auto GetLotSize(int32_t token_) -> int32_t;
        static auto GetTickSize(int32_t token_) -> int32_t;
        static auto GetDivisor(int32_t token_) -> int32_t;
        static auto GetFuture(int32_t token_) -> int32_t;
        static auto GetOppositeToken(int32_t token_) -> int32_t;
        static auto GetStrikePrice(int32_t token_) -> float;

        static auto GetInstType(int32_t token_) -> Instrument;
        static auto GetOption(int32_t token_) -> OptionType;
        static auto GetExchange(int32_t token_) -> Exchange;

        static auto GetSymbol(int32_t token_) -> std::string;
        static auto GetSegment(int32_t token_) -> std::string;
        static auto GetName(int32_t token_) -> std::string;
        static auto GetDescription(int32_t token_) -> std::string;

        static auto IsOption(int32_t token_) -> bool;
        static auto IsEquity(int32_t token_) -> bool;
        static auto IsCall(int32_t token_) -> bool;
        static auto IsPut(int32_t token_) -> bool;
        static auto IsFuture(int32_t token_) -> bool;

        static void ExecuteQuery(const std::string& query_);
        static auto GetResultWithIndex(const std::string& query_) -> TableWithColumnIndexT;
        static auto GetResultWithName(const std::string& query_) -> TableWithColumnNameT;

        static auto GetOptionType(const std::string& option_) -> OptionType;
        static auto GetInstrumentType(const std::string& type_) -> Instrument;
        static auto GetExchange(const std::string& exchange_) -> Exchange;
    };

}  // namespace Lancelot

#endif  // LANCELOT_INCLUDE_CONTRACT_INFO_HPP
