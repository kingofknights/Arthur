#pragma once

#include <boost/asio.hpp>
#include <functional>

#include "Structure.hpp"

class ManualOrder;
using ManualOrderPtrT			  = std::shared_ptr<ManualOrder>;
using CancelPendingOrderFunctionT = std::function<void(OrderInfoPtrT)>;
class PendingBook {
public:
	explicit PendingBook(const ManualOrderPtrT &manualOrder_, boost::asio::io_context::strand &strand_);

	void paint(bool *show_);
	void Insert(const OrderInfoPtrT &tradeInfo_, bool insert_);
	void cancelOrderFunctionCallback(CancelPendingOrderFunctionT cancelPendingOrderFunction_);

protected:
	void DrawPendingBook(bool *show_);
	void Update(const OrderInfoPtrT &tradeInfo_, bool insert_);
	void DrawManualOrderRequestedForCancel();

private:
	CancelPendingOrderFunctionT			 _cancelPendingOrderFunction;
	ManualOrderPtrT						 _manualOrderPtr;
	PendingBookContainerT				 _container;
	PendingOrderUpdateT					 _pendingOrderUpdate;
	BookOrderListT						 _cancelOrder;
	std::unordered_map<int, std::string> _hashing;

	int	 _buyCount		   = 0;
	int	 _sellCount		   = 0;
	int	 _selectedRow	   = -1;
	bool _closeCancelPopup = false;

	ImGuiListClipper				 _clipper;
	boost::asio::io_context::strand &_strand;
};
