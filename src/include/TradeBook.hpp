#pragma once

#include "Structure.hpp"

class TradeBook {
public:
	void paint(bool *show_);
	void Insert(const OrderInfoPtrT &orderInfo_);

protected:
	void DrawTradeBookTable(bool *show_);

private:
	int	   _selectedRow = -1;
	int	   _totalBuy	= 0;
	int	   _totalSell	= 0;
	double _buyValue	= 0;
	double _sellValue	= 0;
	double _netValue	= 0;

	PendingTradeUpdateT _pendingTradeUpdate;
	ImGuiListClipper	_clipper;
	BookOrderListT		_container;
};
