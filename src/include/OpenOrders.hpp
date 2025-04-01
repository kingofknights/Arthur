#pragma once

#include "Arthur_Fwd.hpp"
#include "Structure.hpp"

class OpenOrders {
    using FunctionT = std::function<void(OrderInfoPtrT)>;

  public:
    OpenOrders(const OrderFormPtrT& manualOrder_, ExecutorStrandT& strand_, bool& show_, FunctionT function_);

    void Paint() noexcept;

    void Insert(const OrderInfoPtrT& tradeInfo_, bool insert_);

  protected:
    void DrawPendingBook(bool* show_);

    void Update(const OrderInfoPtrT& tradeInfo_, bool insert_);

    void DrawManualOrderRequestedForCancel();

  private:
    const OrderFormPtrT& _manualOrder;
    const FunctionT      _function;
    ExecutorStrandT&     _strand;

    bool& _show;

    PendingBookContainerT                _container;
    PendingOrderUpdateT                  _pendingOrderUpdate;
    BookOrderListT                       _cancelOrder;
    std::unordered_map<int, std::string> _hashing;

    int  _buyCount         = 0;
    int  _sellCount        = 0;
    int  _selectedRow      = -1;
    bool _closeCancelPopup = false;

    ImGuiListClipper _clipper;
};
