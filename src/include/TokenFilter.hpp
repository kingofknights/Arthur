#pragma once

#include "ContractInfo.hpp"
#include "Enums.hpp"

class TokenFilter {
  public:
    TokenFilter();

    void Paint(bool& show_, std::string& contract_);

    void DrawExchangeFilter() noexcept;
    void DrawInstrumentFilter() noexcept;
    void DrawSymbolFilter() noexcept;
    void DrawExpiryFilter() noexcept;
    void DrawOptionFilter() noexcept;
    void DrawStikeFilter() noexcept;

    void DrawTokenList(bool& show_, std::string& contract_) noexcept;

  private:
    std::string _exchangeData;
    std::string _instrumentData;
    std::string _symbolData;
    std::string _expiryData;
    float       _strikeData = 0;
    std::string _optionData;

    std::vector<Lancelot::Exchange>   _exchange;
    std::vector<Lancelot::Instrument> _instrument;
    std::vector<std::string>          _symbol;
    std::vector<std::string>          _expiry;
    std::vector<float>                _strike;
    std::vector<Lancelot::OptionType> _option;

    Lancelot::ResultSetContainerT&       _container;
    std::vector<Lancelot::ResultSetPtrT> _localContainer;
};
