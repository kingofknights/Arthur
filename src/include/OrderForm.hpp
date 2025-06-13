#pragma once

#include "Arthur_Fwd.hpp"
#include "include/Structure.hpp"

#define NEW_ORDER_WINDOW    "Order Form Window"
#define MODIFY_ORDER_WINDOW "Modify Order Window"

class OrderForm final {
    using FunctionT = std::function<void(OrderFormInfoT, Lancelot::RequestType)>;

  public:
    static auto RoundUp(int numToRound_, int multiple_) -> int;

    OrderForm(ExecutorStrandT& strand_, FunctionT function_);

    void Paint(const char* name_);

    void Update(OrderFormInfoT& info_);

  protected:
    void SentToBroker();

    void DrawInputItem();

  private:
    ExecutorStrandT& _strand;
    FunctionT        _function;

    double _price     = 0;
    double _precision = 1;
    bool   _repeater  = false;

    Lancelot::ResultSetPtrT _resultSet;

    OrderFormInfoT     _order;
    ImVec4             _color;
    ImVec4             _textColor;
    std::string        _clientCode;
    Lancelot::Exchange _exchange;
};
