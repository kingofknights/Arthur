#pragma once

#include "Structure.hpp"

#define ORDER_HISTORY_POPUP_WINDOW "Order History Window"
class OrderHistory : public Singleton {
public:
	static OrderHistory &Instance();

	void paint(bool *show_);
	void LoadOrderHistory(double orderNumber_);

protected:
	void DrawOrderHistory();

private:
	BookOrderListT _container;
	bool		   _showOrderHistory = true;
	double		   _orderNumber		 = 0;
	int			   _index			 = 0;
};
