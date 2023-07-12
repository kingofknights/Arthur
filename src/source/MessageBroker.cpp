//
// Created by VIKLOD on 11-03-2023.
//

#include "../include/MessageBroker.hpp"

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Enums.hpp"
#include "../include/Structure.hpp"
#include "../include/Utils.hpp"

MessageBroker::MessageBroker(boost::asio::io_context& ioContext_) : TBaseSocket(ioContext_) {}

void MessageBroker::setCallback(UpdateTradeFunctionT updateTradeFunction_) { _updateTradeFunction = updateTradeFunction_; }

void MessageBroker::process(const char* buffer_, size_t size_) {
	const auto* request = reinterpret_cast<const Lancelot::CommunicationT*>(buffer_);

	char buffer[2048]{};
	int	 size	 = 0;
	int	 success = Lancelot::Decrypt((unsigned char*)request->_encryptMessage, request->_encryptLength, (unsigned char*)buffer, &size);
	if (success == 0) {
		std::stringstream ss;
		ss << (buffer);
		LOG(INFO, "{} {}", __FUNCTION__, ss.str())
		nlohmann::json		  json	   = nlohmann::json::parse(ss);
		const nlohmann::json& response = json.at(JSON_PARAMS);
		switch (request->_query) {
			case Lancelot::ResponseType_PLACED:
			case Lancelot::ResponseType_NEW:
			case Lancelot::ResponseType_REPLACED:
			case Lancelot::ResponseType_CANCELLED:
			case Lancelot::ResponseType_REPLACE_REJECT:
			case Lancelot::ResponseType_CANCEL_REJECT:
			case Lancelot::ResponseType_NEW_REJECT:
			case Lancelot::ResponseType_FILLED: {
				processOrder(response, static_cast<Lancelot::ResponseType>(request->_query));
				break;
			}
			case Lancelot::ResponseType_PENDING:
			case Lancelot::ResponseType_SUBCRIBED:
			case Lancelot::ResponseType_APPLIED:
			case Lancelot::ResponseType_UNSUBSCRIBED:
			case Lancelot::ResponseType_TERMINATED: {
				processStrategy(response, static_cast<Lancelot::ResponseType>(request->_query));
				break;
			}
			case Lancelot::ResponseType_UPDATES: {
				processUpdates(response);
				break;
			}
			case Lancelot::ResponseType_EXCHANGE_DISCONNECT: {
				Utils::ResetPortfolio(StrategyStatus_DISCONNECTED);
				break;
			}
			case Lancelot::ResponseType_TRACKER: {
				break;
			}
		}
	} else {
		LOG(ERROR, "{} {}", __FUNCTION__, "Unable to decompress data!")
	}
}

void MessageBroker::processOrder(const nlohmann::json& input_, Lancelot::ResponseType type_) {
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
	info->Side		   = static_cast<Lancelot::Side>(input_.at(JSON_SIDE).get<int>());
	info->StatusValue  = static_cast<OrderStatus>(type_);
	info->Contract	   = Lancelot::ContractInfo::GetDescription(info->Token);
	info->Time		   = input_.at(JSON_TIME).get<std::string>();
	info->Client	   = input_.at(JSON_CLIENT).get<std::string>();
	info->Message	   = input_.at(JSON_MESSAGE).get<std::string>();
	_updateTradeFunction(info);
}

void MessageBroker::processStrategy(const nlohmann::json& input_, Lancelot::ResponseType type_) {
	int	 pf	 = input_.at(JSON_PF_NUMBER).get<int>();
	auto ptr = Utils::GetStrategyRow(pf);
	if (ptr.has_value()) {
		const auto& strategy = ptr->lock();
		switch (type_) {
			case Lancelot::ResponseType_PENDING: {
				strategy->Status = StrategyStatus_PENDING;
				break;
			}
			case Lancelot::ResponseType_SUBCRIBED: {
				strategy->Status	 = StrategyStatus_ACTIVE;
				strategy->Subscribed = true;
				break;
			}
			case Lancelot::ResponseType_APPLIED: {
				strategy->Status	 = StrategyStatus_APPLIED;
				strategy->Subscribed = true;
				strategy->Changed	 = false;
				break;
			}
			case Lancelot::ResponseType_UNSUBSCRIBED: {
				strategy->Status	 = StrategyStatus_INACTIVE;
				strategy->Subscribed = false;
				break;
			}
			case Lancelot::ResponseType_TERMINATED: {
				strategy->Status	 = StrategyStatus_TERMINATED;
				strategy->Subscribed = false;
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
