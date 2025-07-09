#include "Lancelot.hpp"
#include "Logger.hpp"
#include "Structure.hpp"

#include <nlohmann/json.hpp>

#include <boost/algorithm/string.hpp>

#include <fstream>
#include <string>
namespace Lancelot {
    using ContractColumnMapT = std::map<std::string, uint8_t>;
    auto GetContractColumnMap(const std::string& exchange_) -> ContractColumnMapT {
        std::fstream         file("contract.json");
        const nlohmann::json contracts = nlohmann::json::parse(file);
        file.close();

        const nlohmann::json& exchange   = contracts.at(exchange_);
        const nlohmann::json& column     = exchange.at("column");
        const auto            expiryDiff = exchange["expiry_diff"].get<uint8_t>();
        ContractColumnMapT    columnMap;

        columnMap.emplace("expiry_diff", expiryDiff);

        for (const auto& item : column.items()) {
            columnMap.emplace(item.key(), item.value().get<uint8_t>());
        }
        return columnMap;
    }

    auto GetContractColumnIndex(ContractColumnMapT list_) -> ContractColumnT {
        return ContractColumnT{
            ._token           = list_["token"],
            ._stream          = list_["stream"],
            ._instrument_type = list_["instrument_type"],
            ._symbol          = list_["symbol"],
            ._expiry          = list_["expiry"],
            ._strike          = list_["strike"],
            ._option_type     = list_["option_type"],
            ._lot_size        = list_["lot_size"],
            ._blq             = list_["blq"],
            ._tick_size       = list_["tick_size"],
            ._freeze_quantity = list_["freeze_quantity"],
            ._low_dpr         = list_["low_dpr"],
            ._high_dpr        = list_["high_dpr"],
            ._description     = list_["description"],
            ._future_token    = list_["future_token"],
            ._asset_code      = list_["asset_code"],
            ._expiry_diff     = list_["expiry_diff"],
        };
    }
    void Initialize(const std::string& name_, const std::string& exchange_) {
        std::fstream file(name_, std::ios::in);
        if (not file.is_open()) {
            LOG(INFO, "unable to open contract file {}", name_);
            return;
        }
        TableWithColumnIndexT table;

        while (not file.eof()) {
            std::string data;
            std::getline(file, data);

            if (data.empty() or std::isalpha(data[0]) != 0) {
                continue;
            }
            RowWithColumnIndexT result;
            boost::split(result, data, boost::is_any_of(" ,"));

            table.push_back(result);
        }
        auto contractFetcher  = ContractFetcher("ResultSet.db3");
        auto contractColumMap = GetContractColumnIndex(GetContractColumnMap(exchange_));
        contractFetcher.Insert(table, contractColumMap, exchange_);
    }
}  // namespace Lancelot
auto main(int argc, const char** argv) -> int {
    if (argc < 3) {
        LOG(ERROR, "usage : {} <filename> <exchange>", argv[0]);
        return 0;
    }
    Lancelot::Initialize(argv[1], argv[2]);
}
