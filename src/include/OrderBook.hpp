#pragma once

#include "Structure.hpp"

class OrderBook {
  public:
    OrderBook(const std::string& name_);
    void paint(bool* show_);
    void Insert(const OrderInfoPtrT& orderInfo_);

  protected:
    void DrawOrderBookTable(bool* show_);

  private:
    int                 _selectedRow = -1;
    PendingTradeUpdateT _pendingTradeUpdate;
    ImGuiListClipper    _clipper;
    BookOrderListT      _container;
    std::string         _name;
    std::string         _tableName;
};
