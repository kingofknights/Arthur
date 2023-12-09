#pragma once

#include "Structure.hpp"

class OptionChain {
public:
	OptionChain();
	void paint(bool* show_);
	void SetOptionForFuture(const std::string& contract_);

private:
	void DrawOptionChain(bool* show_);
	void LoadOptions(const std::string& symbol_, uint32_t expiry_, uint32_t futurePrice_, char comparator_, const std::string& order_);

protected:
	OptionChainContainerT _optionChainContainer;
	MarketWatchDataPtrT	  _future;
	std::string			  _symbol;
	std::string			  _expiry;
};
