//
// Created by vikram on 10/10/19.
//

#include "TokenInfo.hpp"

#include "../include/Structure.hpp"
#include "ContractInfo.hpp"
#include "Lancelot/Lancelot.hpp"

extern MarketWatchDatContainerT MarketWatchDatContainer;

double TokenInfo::ATP(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_averageTradePrice;
    }
    return 0;
}

double TokenInfo::PercentChange(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_pchange;
    }
    return 0;
}

double TokenInfo::Open(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_open;
    }
    return 0;
}

double TokenInfo::High(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_high;
    }
    return 0;
}

double TokenInfo::Low(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_low;
    }
    return 0;
}

double TokenInfo::Close(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_close;
    }
    return 0;
}

double TokenInfo::HighDPR(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_highDpr;
    }
    return 0;
}

double TokenInfo::LowDPR(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_lowDpr;
    }
    return 0;
}

double TokenInfo::TotalBuyQuantity(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_totalBuyQuantity;
    }
    return 0;
}

double TokenInfo::TotalSellQuantity(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_totalSellQuantity;
    }
    return 0;
}

double TokenInfo::VolumeTradedToday(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_volumeTradedToday;
    }
    return 0;
}

double TokenInfo::OpenInterest(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_openInterest;
    }
    return 0;
}

double TokenInfo::LTP(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_lastTradePrice;
    }
    return 0;
}

double TokenInfo::LTQ(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_lastTradePrice;
    }
    return 0;
}

double TokenInfo::BidDepth(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_bid[0]._price;
    }
    return 0;
}

double TokenInfo::AskDepth(double token_) {
    const auto iterator = MarketWatchDatContainer.find(token_);
    if (iterator != MarketWatchDatContainer.end()) {
        return iterator->second->_ask[0]._price;
    }
    return 0;
}

double TokenInfo::Expiry(double token_) {
    int expiry = Lancelot::ContractInfo::GetExpiryDate(token_);
    if (expiry) {
        const time_t ts   = (expiry) + 315513000;
        auto         time = *::localtime(&ts);
        return time.tm_mon + 1;
    }
    return 0;
}

double TokenInfo::ExpiryWeek(double token_) {
    int expiry = Lancelot::ContractInfo::GetExpiryDate(token_);
    if (expiry) {
        const time_t ts   = (expiry) + 315513000;
        auto         time = *::localtime(&ts);
        int          week = 0;
        int          wday = time.tm_wday;
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
