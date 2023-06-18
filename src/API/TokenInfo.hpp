#pragma once
#include <string>

namespace TokenInfo {

double ATP(double token_);
double HighDPR(double token_);
double LowDPR(double token_);
double PercentChange(double token_);
double Open(double token_);
double High(double token_);
double Low(double token_);
double Close(double token_);
double TotalBuyQuantity(double token_);
double TotalSellQuantity(double token_);
double VolumeTradedToday(double token_);
double OpenInterest(double token_);
double LTP(double token_);
double LTQ(double token_);
double BidDepth(double token_);
double AskDepth(double token_);
double Expiry(double token_);
double ExpiryWeek(double token_);
double IsCall(double token_);
double IsPut(double token_);
double IsFuture(double token_);

}  // namespace TokenInfo
