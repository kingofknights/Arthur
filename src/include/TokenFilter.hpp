#pragma once

#include "ContractInfo.hpp"
#include "Enums.hpp"

class TokenFilter {
  public:
    TokenFilter();

    void Paint();

  private:
    std::string                          _exchangeData;
    std::vector<Lancelot::Exchange>      _exchange;
    std::vector<Lancelot::Instrument>    _instrument;
    Lancelot::ResultSetContainerT&       _container;
    std::vector<Lancelot::ResultSetPtrT> _localContainer;
};
