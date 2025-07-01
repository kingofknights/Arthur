#pragma once

#include "Structure.hpp"

class TradeHistory {
  public:
    void paint(bool* show_);
    void Insert(const OrderInfoPtrT& orderInfo_);

  protected:
    void DrawTradeBookTable(bool* show_);

    void ContractFilter();

    void PFFilter();

    void FilterOptionsWindows();

    void DoFilter(const OrderInfoPtrT& tradeInfo_);

    void StartNewFilter();

  private:
    int    _selectedRow    = -1;
    int    _totalBuy       = 0;
    int    _totalSell      = 0;
    double _buyValue       = 0;
    double _sellValue      = 0;
    double _netValue       = 0;
    bool   _isFilterActive = false;

    std::unordered_map<int, bool>         _pfFilter;
    std::unordered_map<std::string, bool> _symbolFilter;

    PendingTradeUpdateT _pendingTradeUpdate;
    ImGuiListClipper    _clipper;
    BookOrderListT      _container;
    BookOrderListT      _filterContainer;
};
