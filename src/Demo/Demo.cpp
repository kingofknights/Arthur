//
// Created by VIKLOD on 06-05-2023.
//

#include "Demo.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <Lancelot/Logger/Logger.hpp>
#include <thread>

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Enums.hpp"
#include "../include/Signal.hpp"
#include "../include/Utils.hpp"

extern AddContractToDemoSignalT AddContractToDemoSignal;
extern DemoOrderInfoSignalT		DemoOrderInfoSignal;
extern MarketEventQueueT		MarketEventQueue;

template <typename T>
T Randon(T low, T up) {
	return low + rand() % int(up - low);
}
double Demo::monteCarloEstimate(double lowBound, double upBound, int iterations) {
	double totalSum = 0;
	int	   loop		= iterations;
	while (loop--) {
		double randNum = lowBound + Randon(0.0, upBound - lowBound);
		totalSum += randNum;
	}

	double value = totalSum / iterations;
	value		 = int(value / 0.0500000f) * 0.05000000f;
	return value;
}
Demo::Demo() : _startOrStop(false) {
	AddContractToDemoSignal.connect([this](int token_) { addContract(token_); });
}
void Demo::GenerateMonteCarloData(const MarketWatchDataPtrT& ptr_) {
	float ltp			 = ptr_->LastTradePrice;
	float topBid		 = ptr_->Bid[0].Price;
	float topAsk		 = ptr_->Ask[0].Price;
	ptr_->LastTradePrice = monteCarloEstimate(ptr_->LowPrice, ptr_->HighPrice, 1000);
	for (int i = 0; i < 5; ++i) {
		ptr_->Bid[i].Price	  = ptr_->LastTradePrice - (i + 1) * 0.05f;
		ptr_->Ask[i].Price	  = ptr_->LastTradePrice + (i + 1) * 0.05f;
		ptr_->Bid[i].Quantity = Randon(0, 100);
		ptr_->Ask[i].Quantity = Randon(0, 100);
		ptr_->Bid[i].Order	  = Randon(0, 100);
		ptr_->Ask[i].Order	  = Randon(0, 100);
	}
	ptr_->AverageTradePrice = ltp;

	ptr_->PercentageChange	= (ptr_->LastTradePrice - ptr_->ClosePrice) / ptr_->ClosePrice * 100.0f;
	ptr_->TotalBuyQuantity	= std::rand();
	ptr_->TotalSellQuantity = std::rand();
	ptr_->Color.LTP			= ptr_->LastTradePrice > ltp;
	ptr_->Color.ATP			= ptr_->LastTradePrice > ltp;
	ptr_->Color.TopBid		= ptr_->Bid[0].Price > topBid;
	ptr_->Color.TopAsk		= ptr_->Ask[0].Price > topAsk;
	std::string time		= fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(0)));
	std::memcpy(ptr_->LastTradeTime.data(), time.c_str(), time.length());
}
void Demo::addContract(int token_) {
	_liveContainer.insert(ContractInfo::GetLiveDataRef(token_));
}

void Demo::Run(std::stop_token& token_) {
	while (not token_.stop_requested()) {
		if (not _startOrStop) continue;

#ifdef ORDER_BOOK
		for (const auto status : {OrderStatus_NEW, OrderStatus_REPLACED, OrderStatus_FILLED, OrderStatus_CANCEL_REJECT}) {
			int gateway = 1;
			for (const auto& side : {Side_BUY, Side_SELL}) {
#endif
				for (const auto& ptr : _liveContainer) {
					GenerateMonteCarloData(ptr);
#ifdef ORDER_BOOK
					MarketEventQueue.push(ptr);
					DemoOrderInfoSignal(GenerateOrderInfo(ptr, gateway, side, status));
					std::this_thread::sleep_for(std::chrono::milliseconds(10));

					++gateway;
					if (token_.stop_requested()) {
						LOG(WARNING, "{} {}", __FUNCTION__, "Exiting")
						return;
					}
				}
			}
#endif
		}
		// break;
	}
	LOG(WARNING, "{} {}", __FUNCTION__, "Exiting")
}

OrderInfoPtrT Demo::GenerateOrderInfo(const MarketWatchDataPtrT& ptr_, int gateway_, Lancelot::Side side_, OrderStatus orderStatus_) {
	auto info		   = std::make_shared<OrderInfoT>();
	info->PF		   = 9999;
	info->Gateway	   = gateway_;
	info->Token		   = ptr_->Token;
	info->Quantity	   = Randon(1, 100);
	info->OrderNo	   = Randon(1, 10000);
	info->Price		   = monteCarloEstimate(ptr_->LowPrice, ptr_->HighPrice, 100);
	info->FillPrice	   = rand();
	info->FillQuantity = rand();
	info->Remaining	   = rand();
	info->Side		   = side_;
	info->StatusValue  = orderStatus_;
	info->Contract	   = std::string(ptr_->Description.data());
	info->Time		   = Utils::FormatTimeToString(std::chrono::system_clock::now().time_since_epoch().count());
	info->Client	   = "PRO";
	return info;
}
