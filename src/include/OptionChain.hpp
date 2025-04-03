#pragma once

#include "Structure.hpp"

class OptionChain final {
    using OptionChainContainerT = std::map<double, OptionChainRowT>;

  public:
    OptionChain();

    void Paint(bool* show_);

    void SetOptionForFuture(const std::string& contract_);

  protected:
    void DrawOptionChain(bool* show_);

    void LoadOptions(const std::string& symbol_, uint32_t expiry_, uint32_t futurePrice_, char comparator_, const std::string& order_);

  private:
    OptionChainContainerT _optionChainContainer;
    MarketWatchDataPtrT   _future;
    std::string           _symbol;
    std::string           _expiry;
};
