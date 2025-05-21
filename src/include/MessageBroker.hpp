//
// Created by VIKLOD on 11-03-2023.
//

#pragma once

#include "Arthur_Fwd.hpp"
#include "BaseSocket.hpp"

#include <nlohmann/json_fwd.hpp>

class MessageBroker final : public TBaseSocket {
    using FunctionT  = std::function<void(OrderInfoPtrT)>;
    using ContainerT = std::unordered_map<int, OrderInfoPtrT>;

  public:
    explicit MessageBroker(ExecutorT& executor_, int16_t user_, FunctionT function_);

  protected:
    void ConnectedStatus(bool status_) noexcept override;

    void Process(const char* buffer_, size_t size_) override;

    void ProcessOrder(const char* buffer_);

    void ProcessTrade(const char* buffer_);

    static void ProcessStrategy(uint32_t pf_, Lancelot::ResponseType type_);

    static void ProcessUpdates(uint32_t pf_, const nlohmann::json& input_);

  private:
    FunctionT  _function;
    ContainerT _container;

    int16_t _user;
};
