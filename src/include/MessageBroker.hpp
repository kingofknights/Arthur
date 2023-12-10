//
// Created by VIKLOD on 11-03-2023.
//

#pragma once

#include <functional>
#include <nlohmann/json.hpp>

#include "BaseSocket.hpp"
#include "Structure.hpp"

using UpdateTradeFunctionT = std::function<void(OrderInfoPtrT)>;

class MessageBroker : public TBaseSocket {
  public:
    MessageBroker(boost::asio::io_context& ioContext_);

    void setCallback(UpdateTradeFunctionT updateTradeFunction_);

  protected:
    void process(const char* buffer_, size_t size_) override;

    void processOrder(const nlohmann::json& input_, Lancelot::ResponseType type_);

    static void processStrategy(const nlohmann::json& input_, Lancelot::ResponseType type_);

    static void processUpdates(const nlohmann::json& input_);

    UpdateTradeFunctionT _updateTradeFunction;

  private:
    GlobalOrderInfoContainerT _globalOrderInfoContainer;
};
