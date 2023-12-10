#include "ContractInfo.hpp"

#include "../include/Structure.hpp"

extern MarketWatchDatContainerT MarketWatchDatContainer;

MarketWatchDataPtrT ContractInfo::GetLiveDataRef(uint32_t token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.cend()) {
        return iterator->second;
    }
    return nullptr;
}
