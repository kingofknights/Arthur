//
// Created by VIKLOD on 11-03-2023.
//

#include "MessageBroker.hpp"

#include "API/Common.hpp"
#include "API/ContractInfo.hpp"
#include "Arthur_Fwd.hpp"
#include "Enums.hpp"
#include "Lancelot/Lancelot.hpp"
#include "Structure.hpp"
#include "Utils.hpp"

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time.hpp>

namespace {
    boost::posix_time::ptime as_ptime(uintmax_t ns) {
        return {{1970, 1, 1}, boost::posix_time::microseconds(ns / 1000)};
    }

    auto TimeStampToHReadble(time_t rawtime_) -> std::string {
        std::string time = boost::posix_time::to_iso_extended_string(as_ptime(rawtime_ + 1.98e+13));
        boost::algorithm::replace_first(time, "T", " ");
        return time;
    }

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
}  // namespace
MessageBroker::MessageBroker(ExecutorT& executor_, int16_t user_, FunctionT function_)
    : TBaseSocket(executor_),
      _function(std::move(function_)),
      _user(user_) {}

void MessageBroker::Process(const char* buffer_, size_t size_) {
    const auto* request = reinterpret_cast<const Lancelot::Header*>(buffer_);

    switch (request->_type) {
        case 4002: {
            ProcessOrder(buffer_);
            break;
        }
        case 1050: {
            ProcessTrade(buffer_);
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
            ProcessStrategy(response->_user._portfolio, static_cast<Lancelot::ResponseType>(response->_header._type));
            break;
        }
        case Lancelot::ResponseType_UPDATES: {
            const auto*    response = reinterpret_cast<const Lancelot::StrategyHeader*>(buffer_);
            nlohmann::json json     = nlohmann::json::parse(buffer_ + sizeof(Lancelot::StrategyHeader), buffer_ + sizeof(Lancelot::StrategyHeader) + size_);
            ProcessUpdates(response->_user._portfolio, json);
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

void MessageBroker::ProcessOrder(const char* buffer_) {
    const auto*   response = reinterpret_cast<const Lancelot::HedgeOrderResponse*>(buffer_);
    OrderInfoPtrT info     = std::make_shared<OrderInfoT>();
    info->_portfolio       = response->_user._portfolio,
    info->_uniqueId        = response->_clientOrderNumber,
    info->_token           = response->_token,
    info->_quantity        = response->_quantity,
    info->_fillQuantity    = 0,
    info->_remaining       = response->_quantity,
    info->_orderNumber     = response->_exchangeOrderNumber,
    info->_price           = response->_price / 100.0F,
    info->_fillPrice       = 0,
    info->_side            = static_cast<Lancelot::Side>(response->_side);
    info->_statusValue     = GetResponseStatus(response->_orderStatus);
    info->_contract        = Lancelot::ContractInfo::GetDescription(info->_token);
    info->_time            = TimeStampToHReadble(response->_timestamp);
    info->_client          = FORMAT("{}", response->_user._user);
    info->_message         = FORMAT("{}", response->_errorCode);
    _function(info);
}
void MessageBroker::ProcessTrade(const char* buffer_) {
    const auto*   response = reinterpret_cast<const Lancelot::TradeConfirmation*>(buffer_);
    OrderInfoPtrT info     = std::make_shared<OrderInfoT>();
    info->_portfolio       = response->_portfolio,
    info->_uniqueId        = response->_clientOrderNumber,
    info->_token           = response->_token,
    info->_quantity        = response->_quantity,
    info->_fillQuantity    = response->_fillQuantity,
    info->_remaining       = response->_remainingQuantity,
    info->_orderNumber     = response->_exchangeOrderNumber,
    info->_price           = response->_price / 100.0F,
    info->_fillPrice       = response->_fillPrice / 100.0F,
    info->_side            = static_cast<Lancelot::Side>(response->_side);
    info->_statusValue     = response->_remainingQuantity == 0 ? OrderStatus_FILLED : OrderStatus_PARTIAL_FILLED;
    info->_contract        = Lancelot::ContractInfo::GetDescription(info->_token);
    info->_time            = TimeStampToHReadble(response->_timestamp);
    info->_client          = FORMAT("{}", response->_userId);
    info->_message         = "";
    _function(info);
}

void MessageBroker::ProcessStrategy(uint32_t pf_, Lancelot::ResponseType type_) {
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

void MessageBroker::ProcessUpdates(uint32_t pf_, const nlohmann::json& input_) {
    auto ptr = Utils::GetStrategyRow(pf_);
    if (ptr.has_value() and not ptr->expired()) {
        const auto& strategy = ptr->lock();
        for (const auto& argument : input_.items()) {
            auto iterator = strategy->_parameterInfoList.find(argument.key());
            if (iterator != strategy->_parameterInfoList.end()) {
                if (iterator->second._type == DataType_UPDATES) {
                    iterator->second._parameter._text = argument.value().get<std::string>();
                }
            }
        }
    }
}
void MessageBroker::ConnectedStatus(bool status_) noexcept {
    if (status_) {
        Lancelot::StrategyHeader login{
            ._header = {
                ._type   = Lancelot::RequestType_LOGIN,
                ._length = sizeof(login) - sizeof(Lancelot::Header),
            },
            ._user = {
                ._user      = _user,
                ._portfolio = 0,
            },
        };
        WriteAsync(&login, sizeof(login));
    }
    Utils::ResetPortfolio(status_ ? StrategyStatus_INACTIVE : StrategyStatus_DISCONNECTED);
}
