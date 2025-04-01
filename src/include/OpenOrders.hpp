#pragma once

#include "Arthur_Fwd.hpp"
#include "Structure.hpp"

class OpenOrders {
  public:
    explicit OpenOrders(const OrderFormPtrT& manualOrder_, boost::asio::io_context::strand& strand_);

    void Paint(bool* show_);

    void Insert(const OrderInfoPtrT& tradeInfo_, bool insert_);

    void CancelOrderFunctionCallback(CancelPendingOrderFunctionT cancelPendingOrderFunction_);

  protected:
    void DrawPendingBook(bool* show_);
    void Update(const OrderInfoPtrT& tradeInfo_, bool insert_);
    void DrawManualOrderRequestedForCancel();

  private:
    CancelPendingOrderFunctionT          _cancelPendingOrderFunction;
    OrderFormPtrT                        _manualOrderPtr;
    PendingBookContainerT                _container;
    PendingOrderUpdateT                  _pendingOrderUpdate;
    BookOrderListT                       _cancelOrder;
    std::unordered_map<int, std::string> _hashing;

    int  _buyCount         = 0;
    int  _sellCount        = 0;
    int  _selectedRow      = -1;
    bool _closeCancelPopup = false;

    ImGuiListClipper                 _clipper;
    boost::asio::io_context::strand& _strand;
};
