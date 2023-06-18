//
// Created by vikram on 10/10/19.
//

#include "TokenInfo.hpp"

#include <Lancelot/ContractInfo/ContractInfo.hpp>

#include "../include/Structure.hpp"
#include "ContractInfo.hpp"

extern MarketWatchDatContainerT MarketWatchDatContainer;

double TokenInfo::ATP(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->AverageTradePrice;
	}
	return 0;
}

double TokenInfo::PercentChange(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->PercentageChange;
	}
	return 0;
}

double TokenInfo::Open(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->OpenPrice;
	}
	return 0;
}

double TokenInfo::High(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->HighPrice;
	}
	return 0;
}

double TokenInfo::Low(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->LowPrice;
	}
	return 0;
}

double TokenInfo::Close(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->ClosePrice;
	}
	return 0;
}

double TokenInfo::HighDPR(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->HighDPR;
	}
	return 0;
}

double TokenInfo::LowDPR(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->LowDPR;
	}
	return 0;
}

double TokenInfo::TotalBuyQuantity(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->TotalBuyQuantity;
	}
	return 0;
}

double TokenInfo::TotalSellQuantity(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->TotalSellQuantity;
	}
	return 0;
}

double TokenInfo::VolumeTradedToday(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->VolumeTradedToday;
	}
	return 0;
}

double TokenInfo::OpenInterest(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->OpenInterest;
	}
	return 0;
}

double TokenInfo::LTP(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->LastTradePrice;
	}
	return 0;
}

double TokenInfo::LTQ(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->LastTradeQuantity;
	}
	return 0;
}

double TokenInfo::BidDepth(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->Bid[0].Price;
	}
	return 0;
}

double TokenInfo::AskDepth(double token_) {
	const auto iterator = MarketWatchDatContainer.find(token_);
	if (iterator != MarketWatchDatContainer.end()) {
		return iterator->second->Ask[0].Price;
	}
	return 0;
}

double TokenInfo::Expiry(double token_) {
	int expiry = Lancelot::ContractInfo::GetExpiryDate(token_);
	if (expiry) {
		const time_t ts	  = (expiry) + 315513000;
		auto		 time = *std::localtime(&ts);
		return time.tm_mon + 1;
	}
	return 0;
}

double TokenInfo::ExpiryWeek(double token_) {
	int expiry = Lancelot::ContractInfo::GetExpiryDate(token_);
	if (expiry) {
		const time_t ts	  = (expiry) + 315513000;
		auto		 time = *std::localtime(&ts);
		int			 week = 0;
		int			 wday = time.tm_wday;
		for (int i = time.tm_mday; i > 0; --i) {
			if (wday == 5) week += 1;
			wday -= 1;
			if (wday == -1) wday = 6;
		}
		return week + 1;
	}
	return 0;
}

double TokenInfo::IsCall(double token_) {
	return Lancelot::ContractInfo::IsCall(token_);
}

double TokenInfo::IsPut(double token_) {
	return Lancelot::ContractInfo::IsPut(token_);
}

double TokenInfo::IsFuture(double token_) {
	return Lancelot::ContractInfo::IsFuture(token_);
}
