#pragma once

#include "Arthur_Fwd.hpp"
#include "Structure.hpp"

#include <string>
#include <unordered_map>

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

    void ContextMenu(int index_) noexcept;

    void ContractFilter();

    void PFFilter();

  private:
    const OrderFormPtrT& _manualOrder;
    const FunctionT      _function;
    ExecutorStrandT&     _strand;

    bool& _show;

    PendingOrderContainerT _container;
    PendingOrderContainerT _filterContainer;
    PendingOrderUpdateT    _pendingOrderUpdate;
    BookOrderListT         _cancelOrder;

    std::unordered_map<uint32_t, std::string> _hashing;

    int  _buyCount         = 0;
    int  _sellCount        = 0;
    int  _selectedRow      = -1;
    bool _closeCancelPopup = false;

    ImGuiTextFilter  _filter;
    ImGuiListClipper _clipper;

    std::unordered_map<int, bool>         _pfFilter;
    std::unordered_map<std::string, bool> _symbolFilter;
};
