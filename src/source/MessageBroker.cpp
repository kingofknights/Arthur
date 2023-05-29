//
// Created by VIKLOD on 11-03-2023.
//

#include "../include/MessageBroker.hpp"

#include "../API/ContractInfo.hpp"
#include "../DataFeed/Compression.h"
#include "../include/Enums.hpp"
#include "../include/Logger.hpp"
#include "../include/Utils.hpp"

MessageBroker::MessageBroker(boost::asio::io_context& ioContext_) : TBaseSocket(ioContext_) {}

void MessageBroker::setCallback(UpdateTradeFunctionT updateTradeFunction_) { _updateTradeFunction = std::move(updateTradeFunction_); }

void MessageBroker::process(const char* buffer_, size_t size_) {
	const auto* request = reinterpret_cast<const RequestInPackT*>(buffer_);

	char buffer[2048]{};
	int	 size	 = 0;
	int	 success = Compression::DeCompressData((unsigned char*)request->Message.data(), request->CompressedMsgLen, (unsigned char*)buffer, &size);
	if (success == 0) {
		std::istringstream	  ss(buffer);
		nlohmann::json		  json	   = nlohmann::json::parse(ss);
		const nlohmann::json& response = json.at(JSON_PARAMS);
		switch (request->Type) {
			case ResponseType_PLACED:
			case ResponseType_NEW:
			case ResponseType_REPLACED:
			case ResponseType_CANCELLED:
			case ResponseType_REPLACE_REJECT:
			case ResponseType_CANCEL_REJECT:
			case ResponseType_NEW_REJECT:
			case ResponseType_FILLED: {
				processOrder(json, static_cast<ResponseType>(request->Type));
				break;
			}
			case ResponseType_PENDING:
			case ResponseType_SUBCRIBED:
			case ResponseType_APPLIED:
			case ResponseType_UNSUBSCRIBED:
			case ResponseType_TERMINATED: {
				processStrategy(response, static_cast<ResponseType>(request->Type));
				break;
			}
			case ResponseType_UPDATES: {
				processUpdates(response);
				break;
			}
			case ResponseType_EXCHANGE_DISCONNECT: {
				Utils::ResetPortfolio(StrategyStatus_DISCONNECTED);
				break;
			}
			case ResponseType_TRACKER: {
				break;
			}
		}
	} else {
		LOG(ERROR, "{} {}", __FUNCTION__, "Unable to decompress data!")
	}
}

void MessageBroker::processOrder(const nlohmann::json& input_, ResponseType type_) {
	OrderInfoPtrT info = std::make_shared<OrderInfoT>();
	info->PF		   = input_.at(JSON_PF_NUMBER).get<uint32_t>();
	info->Gateway	   = input_.at(JSON_UNIQUE_ID).get<uint32_t>();
	info->Token		   = input_.at(JSON_TOKEN).get<uint32_t>();
	info->Quantity	   = input_.at(JSON_QUANTITY).get<int>();
	info->FillQuantity = input_.at(JSON_FILL_QUANTITY).get<int>();
	info->Remaining	   = input_.at(JSON_REMAINING).get<int>();
	info->OrderNo	   = input_.at(JSON_ORDER_ID).get<long>();
	info->Price		   = input_.at(JSON_PRICE).get<float>();
	info->FillPrice	   = input_.at(JSON_FILL_PRICE).get<float>();
	info->Side		   = static_cast<SideType>(input_.at(JSON_SIDE).get<int>());
	info->StatusValue  = static_cast<OrderStatus>(type_);
	info->Contract	   = ContractInfo::GetFullName(info->Token);
	info->Time		   = input_.at(JSON_TIME).get<std::string>();
	info->Client	   = input_.at(JSON_CLIENT).get<std::string>();
	info->Message	   = input_.at(JSON_MESSAGE).get<std::string>();
	_updateTradeFunction(info);
}

void MessageBroker::processStrategy(const nlohmann::json& input_, ResponseType type_) {
	int	 pf	 = input_.at(JSON_PF_NUMBER).get<int>();
	auto ptr = Utils::GetStrategyRow(pf);
	if (ptr.has_value()) {
		const auto& strategy = ptr->lock();
		switch (type_) {
			case ResponseType_PENDING: {
				strategy->Status = StrategyStatus_PENDING;
				break;
			}
			case ResponseType_SUBCRIBED: {
				strategy->Status = StrategyStatus_ACTIVE;
				break;
			}
			case ResponseType_APPLIED: {
				strategy->Status = StrategyStatus_APPLIED;
				break;
			}
			case ResponseType_UNSUBSCRIBED: {
				strategy->Status = StrategyStatus_INACTIVE;
				break;
			}
			case ResponseType_TERMINATED: {
				strategy->Status = StrategyStatus_TERMINATED;
				break;
			}
		}
	}
}

void MessageBroker::processUpdates(const nlohmann::json& input_) {
	int	 pf	 = input_.at(JSON_PF_NUMBER).get<int>();
	auto ptr = Utils::GetStrategyRow(pf);
	if (ptr.has_value() and not ptr->expired()) {
		const auto& strategy  = ptr->lock();
		const auto& arguments = input_.at(JSON_ARGUMENTS);
		for (const auto& argument : arguments.items()) {
			auto iterator = strategy->ParameterInfoList.find(argument.key());
			if (iterator != strategy->ParameterInfoList.end()) {
				if (iterator->second.Type == DataType_UPDATES) {
					iterator->second.Parameter.Text = argument.value().get<std::string>();
				}
			}
		}
	}
}
