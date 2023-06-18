#pragma once
#include <boost/asio.hpp>
#include <functional>

#include "Structure.hpp"

#define NEW_ORDER_WINDOW "New Manual Order Window"
#define MODIFY_ORDER_WINDOW "Modify Manual Order Window"

enum RequestType;

namespace Lancelot {
enum Exchange;

}
using PublishOrderFunctionT = std::function<void(ManualOrderInfoT, RequestType)>;
class ManualOrder final {
public:
	explicit ManualOrder(boost::asio::io_context::strand& strand_);

	void paint(const char* name_);
	void Update(ManualOrderInfoT& info_);
	void publishOrderCallback(PublishOrderFunctionT publishOrderFunction_);

private:
	void SentToBroker();
	void DrawInputItem();

	ManualOrderInfoT   _order;
	ImVec4			   _color;
	std::string		   _clientCode;
	Lancelot::Exchange _exchange;

	boost::asio::io_context::strand& _strand;
	PublishOrderFunctionT			 _publishOrderFunction;
};
