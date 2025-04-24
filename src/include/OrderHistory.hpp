#pragma once

#include "Structure.hpp"

#include <cstdint>

#define ORDER_HISTORY_POPUP_WINDOW "Order History Window"
class OrderHistory : public Singleton {
  public:
    static auto Instance() -> OrderHistory&;

    void Paint(bool* show_);

    void LoadOrderHistory(uint64_t orderNumber_);

  protected:
    void DrawOrderHistory();

  private:
    BookOrderListT _container;
    bool           _showOrderHistory = true;
    uint64_t       _orderNumber      = 0;
    int            _index            = 0;
};
