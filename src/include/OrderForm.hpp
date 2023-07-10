#pragma once
#include <boost/asio.hpp>
#include <functional>

#include "Structure.hpp"

#define NEW_ORDER_WINDOW	"Order Form Window"
#define MODIFY_ORDER_WINDOW "Modify Order Window"

namespace Lancelot {
enum Exchange : int;
enum RequestType : int;
}  // namespace Lancelot

using PublishOrderFunctionT = std::function<void(OrderFormInfoT, Lancelot::RequestType)>;
class OrderForm final {
public:
	explicit OrderForm(boost::asio::io_context::strand& strand_);

	void paint(const char* name_);
	void Update(OrderFormInfoT& info_);
	void publishOrderCallback(PublishOrderFunctionT publishOrderFunction_);

private:
	void SentToBroker();
	void DrawInputItem();

	OrderFormInfoT	   _order;
	ImVec4			   _color;
	std::string		   _clientCode;
	Lancelot::Exchange _exchange;

	boost::asio::io_context::strand& _strand;
	PublishOrderFunctionT			 _publishOrderFunction;
};
