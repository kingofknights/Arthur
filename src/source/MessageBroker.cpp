//
// Created by VIKLOD on 11-03-2023.
//

#include "../include/MessageBroker.hpp"

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Enums.hpp"
#include "../include/Structure.hpp"
#include "../include/Utils.hpp"
#include "Lancelot/Lancelot.hpp"
#include "Structure.hpp"

#include <utility>

constexpr auto GetResponseStatus(int response_) noexcept -> OrderStatus {
    switch (response_) {
        case 10:
            return OrderStatus_PLACED;
        case 20:
            return OrderStatus_NEW;
        case 21:
            return OrderStatus_REPLACED;
        case 22:
            return OrderStatus_CANCELLED;
        case 30:
            return OrderStatus_NEW_REJECT;
        case 31:
            return OrderStatus_REPLACE_REJECT;
        case 32:
            return OrderStatus_CANCEL_REJECT;
        case 40:
            return OrderStatus_PARTIAL_FILLED;
        case 41:
            return OrderStatus_FILLED;
    }
    return OrderStatus_NEW_REJECT;
}
MessageBroker::MessageBroker(boost::asio::io_context& ioContext_)
    : TBaseSocket(ioContext_) {}

void MessageBroker::setCallback(UpdateTradeFunctionT updateTradeFunction_) { _updateTradeFunction = std::move(updateTradeFunction_); }

void MessageBroker::Process(const char* buffer_, size_t size_) {
    const auto* request = reinterpret_cast<const Lancelot::Header*>(buffer_);

    switch (request->_type) {
        case 4002: {
            processOrder(buffer_);
            break;
        }
        case Lancelot::ResponseType_FILLED: {
            break;
        }
        case Lancelot::ResponseType_PENDING:
        case Lancelot::ResponseType_SUBCRIBED:
        case Lancelot::ResponseType_APPLIED:
        case Lancelot::ResponseType_UNSUBSCRIBED:
        case Lancelot::ResponseType_TERMINATED: {
            const auto* response = reinterpret_cast<const Lancelot::StrategyHeader*>(buffer_);
            processStrategy(response->_user._portfolio, static_cast<Lancelot::ResponseType>(response->_header._type));
            break;
        }
        case Lancelot::ResponseType_UPDATES: {
            processUpdates({});
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
}

void MessageBroker::processOrder(const char* buffer_) {
    const auto*   response = reinterpret_cast<const Lancelot::HedgeOrderResponse*>(buffer_);
    OrderInfoPtrT info     = std::make_shared<OrderInfoT>();
    info->_portfolio       = response->_user._portfolio,
    info->_uniqueId        = response->_clientOrderNumber,
    info->_token           = response->_token,
    info->_quantity        = response->_quantity,
    info->_fillQuantity    = 0,
    info->_remaining       = response->_quantity,
    info->_orderNumber     = response->_exchangeOrderNumber,
    info->_price           = response->_price,
    info->_fillPrice       = 0,
    info->_side            = static_cast<Lancelot::Side>(response->_side);
    info->_statusValue     = GetResponseStatus(response->_orderStatus);
    info->_contract        = Lancelot::ContractInfo::GetDescription(info->_token);
    info->_time            = FORMAT("{}", response->_timestamp);
    info->_client          = FORMAT("{}", response->_user._user);
    info->_message         = FORMAT("{}", response->_errorCode);
    _updateTradeFunction(info);
}

void MessageBroker::processStrategy(uint32_t pf_, Lancelot::ResponseType type_) {
    auto ptr = Utils::GetStrategyRow(pf_);
    if (ptr.has_value()) {
        const auto& strategy = ptr->lock();
        switch (type_) {
            case Lancelot::ResponseType_PENDING: {
                strategy->_status = StrategyStatus_PENDING;
                break;
            }
            case Lancelot::ResponseType_SUBCRIBED: {
                strategy->_status     = StrategyStatus_ACTIVE;
                strategy->_subscribed = true;
                break;
            }
            case Lancelot::ResponseType_APPLIED: {
                strategy->_status     = StrategyStatus_APPLIED;
                strategy->_subscribed = true;
                strategy->_changed    = false;
                break;
            }
            case Lancelot::ResponseType_UNSUBSCRIBED: {
                strategy->_status     = StrategyStatus_INACTIVE;
                strategy->_subscribed = false;
                break;
            }
            case Lancelot::ResponseType_TERMINATED: {
                strategy->_status     = StrategyStatus_TERMINATED;
                strategy->_subscribed = false;
                break;
            }
        }
    }
}

void MessageBroker::processUpdates(const nlohmann::json& input_) {
    int  pf  = input_.at(JSON_PF_NUMBER).get<int>();
    auto ptr = Utils::GetStrategyRow(pf);
    if (ptr.has_value() and not ptr->expired()) {
        const auto& strategy  = ptr->lock();
        const auto& arguments = input_.at(JSON_ARGUMENTS);
        for (const auto& argument : arguments.items()) {
            auto iterator = strategy->_parameterInfoList.find(argument.key());
            if (iterator != strategy->_parameterInfoList.end()) {
                if (iterator->second._type == DataType_UPDATES) {
                    iterator->second._parameter._text = argument.value().get<std::string>();
                }
            }
        }
    }
}
