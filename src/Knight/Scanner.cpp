#include "Scanner.hpp"

#include <Lancelot/Logger/Logger.hpp>
#include <boost/preprocessor.hpp>

#include "../API/BaseScanner.hpp"
#include "../API/TokenInfo.hpp"
#include "../include/Utils.hpp"

extern ScannerContainerT ScannerContainer;

#define STR(X) #X
#define TO_STRING(X) STR(X)
#define FUNCTION(R, _, T) AddFunction(TO_STRING(T), TokenInfo::T);

#define PARAM_FUNCTION_LIST_2                                                                                                                                                                    \
	BOOST_PP_TUPLE_TO_LIST(23, (ATP, PercentChange, Open, High, Low, Close, HighDPR, LowDPR, TotalBuyQuantity, TotalSellQuantity, VolumeTradedToday, OpenInterest, LTP, LTQ, BidDepth, AskDepth, \
								Expiry, ExpiryWeek, IsCall, IsPut, IsFuture))

Scanner::Scanner() {
	BOOST_PP_LIST_FOR_EACH(FUNCTION, _, PARAM_FUNCTION_LIST_2)
	AddFunction("ScannerAPI", Utils::ScannerAPI);
}

void Scanner::Process(const int Token) {
	if (_lock.try_lock()) {
		std::for_each(ExpressionContainer.begin(), ExpressionContainer.end(), [this, Token](auto&& PH1) { Transfer(Token, std::forward<decltype(PH1)>(PH1)); });
		_lock.unlock();
	}
}

void Scanner::Transfer(int Token, const ExpressionContainerT::value_type& value) {
	setToken(Token);
	GetOutput(value.second);
}

Scanner& Scanner::GetInstance() {
	static Scanner scanner;
	return scanner;
}

bool Scanner::EvaluateExp(uint64_t key, const std::string& formula, bool oneShot) {
	LOG(INFO, "Scanner EvaluateExp: {} {} {}", key, formula, oneShot)
	return ParseExpression(key, formula, oneShot);
}

void Scanner::ScannerUnsubscribe(uint64_t key) {
	{
		auto iterator = this->ExpressionContainer.find(key);
		if (iterator != this->ExpressionContainer.end()) {
			iterator->second.release();
			this->ExpressionContainer.erase(iterator);
		}
	}

	{
		auto iterator = ScannerContainer.find(key);
		if (iterator != ScannerContainer.end()) {
			if (iterator->second.BaseScannerPtr) {
				iterator->second.BaseScannerPtr->StopThat();
				iterator->second.BaseScannerPtr.reset();
			}
			iterator->second.Library.unload();
			ScannerContainer.erase(iterator);
		}
	}
}
