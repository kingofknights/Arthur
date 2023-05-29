#pragma once
#include <memory>
#include <string>

struct ResultSetT;
struct MarketWatchDataT;
using MarketWatchDataPtrT = std::shared_ptr<MarketWatchDataT>;

namespace ContractInfo {

MarketWatchDataPtrT GetLiveDataRef(uint32_t token_);

std::string GetFullName(uint32_t token_);
std::string GetExchange(uint32_t token_);
std::string GetName(uint32_t token_);
std::string GetSymbol(uint32_t token_);
std::string GetInstType(uint32_t token_);
std::string GetSegment(uint32_t token_);

uint32_t GetOppositeToken(uint32_t token_);
uint32_t GetToken(const std::string& name_);
uint32_t GetExpiry(uint32_t token_);
uint32_t GetTickSize(uint32_t token_);
uint32_t GetFuture(uint32_t token_);

bool IsOption(uint32_t token_);
bool IsEquity(uint32_t token_);
bool IsCall(uint32_t token_);
bool IsPut(uint32_t token_);
bool IsFuture(uint32_t token_);

float GetStrikePrice(uint32_t token_);
int	  GetLotSize(uint32_t token_);
int	  GetLotMultiple(uint32_t token_);

ResultSetT GetResultSet(uint32_t token_);
}  // namespace ContractInfo
