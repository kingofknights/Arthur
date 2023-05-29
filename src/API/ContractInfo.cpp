#include "ContractInfo.hpp"

#include "../include/Logger.hpp"
#include "../include/Sqlite.hpp"
#include "../include/StoreProcedures.hpp"
#include "../include/Structure.hpp"

extern MarketWatchDatContainerT MarketWatchDatContainer;
extern NameToTokenContainerT	NameToTokenContainer;
extern ResultSetContainerT		ResultSetContainer;
extern TokenToFutureTokenT		TokenToFutureToken;

MarketWatchDataPtrT ContractInfo::GetLiveDataRef(uint32_t token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.cend()) {
		return iterator->second;
	}
	return nullptr;
}

uint32_t ContractInfo::GetToken(const std::string& name_) {
	const auto iterator = NameToTokenContainer.find(name_);
	if (iterator != NameToTokenContainer.cend()) {
		return iterator->second;
	}
	return -1;
}

std::string ContractInfo::GetExchange(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.Exchange;
	}
	return {"No Exchange"};
}

std::string ContractInfo::GetName(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.Name;
	}
	return {"No Name"};
}

std::string ContractInfo::GetSymbol(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.Symbol;
	}
	return {"No Symbol"};
}

uint32_t ContractInfo::GetExpiry(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.ExpiryDate;
	}
	return {0};
}

std::string ContractInfo::GetInstType(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.InstType;
	}
	return {"No InstType"};
}

std::string ContractInfo::GetSegment(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.Segment;
	}
	return {"No Segment"};
}

float ContractInfo::GetStrikePrice(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.StrikePrice;
	}
	return 0;
}

int ContractInfo::GetLotSize(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.LotSize;
	}
	return 0;
}

int ContractInfo::GetLotMultiple(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.LotMultiple;
	}
	return 0;
}

uint32_t ContractInfo::GetTickSize(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.TickSize;
	}
	return 0;
}

bool ContractInfo::IsFuture(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.end()) {
		return iterator->second.InstType.substr(0, 3) == "FUT";
	}
	return false;
}

bool ContractInfo::IsOption(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.end()) {
		return iterator->second.InstType.substr(0, 3) == "OPT";
	}
	return false;
}

bool ContractInfo::IsEquity(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.end()) {
		return iterator->second.InstType == "Equity";
	}
	return false;
}

bool ContractInfo::IsCall(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.OptionType == "CE";
	}
	return false;
}

bool ContractInfo::IsPut(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.OptionType == "PE";
	}
	return false;
}

std::string ContractInfo::GetFullName(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second.FullName;
	}
	return {"No FullName"};
}

uint32_t ContractInfo::GetOppositeToken(uint32_t token_) {
	std::string		  name	   = GetName(token_);
	bool			  isCall_  = IsCall(token_);
	const std::string call_put = isCall_ ? "CE" : "PE";
	const std::string reverse  = isCall_ ? "PE" : "CE";
	size_t			  pos	   = name.find(call_put);
	if (pos != std::string::npos) {
		name.replace(pos, call_put.length(), reverse);
		return GetToken(name);
	}
	return 0;
}

ResultSetT ContractInfo::GetResultSet(uint32_t token_) {
	const auto iterator = ResultSetContainer.find(token_);
	if (iterator != ResultSetContainer.cend()) {
		return iterator->second;
	}
	return {};
}
uint32_t ContractInfo::GetFuture(uint32_t token_) {
	const auto iterator = TokenToFutureToken.find(token_);
	if (iterator != TokenToFutureToken.cend()) {
		return iterator->second;
	}
	return token_;
}
