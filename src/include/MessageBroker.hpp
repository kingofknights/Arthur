//
// Created by VIKLOD on 11-03-2023.
//

#pragma once

#include "BaseSocket.hpp"
#include "Structure.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <functional>

using UpdateTradeFunctionT = std::function<void(OrderInfoPtrT)>;

class MessageBroker : public TBaseSocket {
  public:
    MessageBroker(boost::asio::io_context& ioContext_);

    void setCallback(UpdateTradeFunctionT updateTradeFunction_);

  protected:
    void Process(const char* buffer_, size_t size_) override;

    void processOrder(const char* buffer_);

    static void processStrategy(uint32_t pf_, Lancelot::ResponseType type_);

    static void processUpdates(const nlohmann::json& input_);

    UpdateTradeFunctionT _updateTradeFunction;

  private:
    GlobalOrderInfoContainerT _globalOrderInfoContainer;
};
