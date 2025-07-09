#pragma once

#include "Structure.hpp"
#include "Utils.hpp"

class TradeHistory {
  public:
    void paint(bool* show_);
    void Insert(const OrderInfoPtrT& orderInfo_);

  protected:
    void DrawTradeBookTable(bool* show_);

    void ContractFilter();

    void PFFilter();

    void FilterOptionsWindows();

    void StartNewFilter();

    auto IsFilterActive() -> bool;

    void FillFilterOption();

  private:
    int    _selectedRow    = -1;
    int    _totalBuy       = 0;
    int    _totalSell      = 0;
    double _buyValue       = 0;
    double _sellValue      = 0;
    double _netValue       = 0;
    bool   _isFilterActive = false;

    PendingTradeUpdateT         _pendingTradeUpdate;
    ImGuiListClipper            _clipper;
    BookOrderListT              _container;
    BookOrderListT              _filterContainer;
    ColumnFilterActiveT         _active;
    ColumnFilterContainerArrayT _filter;
    int                         _currentFilterOpended = -1;
    bool                        _filterWindowShow     = false;
};
