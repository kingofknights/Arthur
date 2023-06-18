#pragma once
#include <memory>
#include <string>

struct ResultSetT;
struct MarketWatchDataT;
using MarketWatchDataPtrT = std::shared_ptr<MarketWatchDataT>;

namespace ContractInfo {

MarketWatchDataPtrT GetLiveDataRef(uint32_t token_);

}  // namespace ContractInfo
