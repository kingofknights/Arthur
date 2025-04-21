#pragma once

#include "Arthur_Fwd.hpp"
#include "Structure.hpp"

class OpenOrders {
    using FunctionT              = std::function<void(OrderInfoPtrT)>;
    using PendingOrderContainerT = std::map<std::string, OrderInfoPtrT>;
    using PendingOrderUpdateT    = QueueT<std::pair<OrderInfoPtrT, bool>>;

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

    PendingOrderContainerT _container;
    PendingOrderUpdateT    _pendingOrderUpdate;
    BookOrderListT         _cancelOrder;

    std::unordered_map<uint32_t, std::string> _hashing;

    int  _buyCount         = 0;
    int  _sellCount        = 0;
    int  _selectedRow      = -1;
    bool _closeCancelPopup = false;

    ImGuiListClipper _clipper;
};
