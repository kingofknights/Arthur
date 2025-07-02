#ifndef LANCELOT_INCLUDE_CONTRACT_FETCHER_HPP
#define LANCELOT_INCLUDE_CONTRACT_FETCHER_HPP
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace SQLite {
    class Database;
};
namespace Lancelot {
    using RowWithColumnNameT   = std::unordered_map<std::string, std::string>;
    using RowWithColumnIndexT  = std::vector<std::string>;
    using TableWithColumnNameT = std::vector<RowWithColumnNameT>;

    struct ContractColumnT {
        uint16_t _token;
        uint16_t _stream;
        uint16_t _instrument_type;
        uint16_t _symbol;
        uint16_t _expiry;
        uint16_t _strike;
        uint16_t _option_type;
        uint16_t _lot_size;
        uint16_t _blq;
        uint16_t _tick_size;
        uint16_t _freeze_quantity;
        uint16_t _low_dpr;
        uint16_t _high_dpr;
        uint16_t _description;
        uint16_t _future_token;
        uint16_t _asset_code;
        uint16_t _expiry_diff;
    };
    using TableWithColumnIndexT = std::vector<RowWithColumnIndexT>;

    class ContractFetcher final {
      public:
        explicit ContractFetcher(const std::string& name_);

        ~ContractFetcher() noexcept;

        void Insert(TableWithColumnIndexT& table_, ContractColumnT column_, std::string exchange_);

        void ExecuteQuery(const std::string& query_);

        auto GetResultWithColumnName(const std::string& query_) -> TableWithColumnNameT;

        auto GetResultWithColumnIndex(const std::string& query_) -> TableWithColumnIndexT;

        bool IsTableExist(const std::string& name_);

      private:
        static void PrintMetaData(const std::string& name_);

        SQLite::Database* _database;
    };
}  // namespace Lancelot

#endif  // LANCELOT_INCLUDE_CONTRACT_FETCHER_HPP
