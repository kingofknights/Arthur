#pragma once

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <memory>
#include <thread>
#include <vector>

namespace Lancelot {
    struct Header;
}

class TemplateBuilder;
class ExcelWindow;
class Position;
class MarketWatch;
class OrderForm;
class MulticastReceiver;
class OpenOrders;
class StrategyWorkspace;
class TradeHistory;
class MessageBroker;
class OptionChain;
class OrderBook;

struct OrderInfoT;
struct OrderFormInfoT;
struct StrategyRowT;

using OrderInfoPtrT         = std::shared_ptr<OrderInfoT>;
using StrategyRowPtrT       = std::shared_ptr<StrategyRowT>;
using TemplateBuilderPtrT   = std::unique_ptr<TemplateBuilder>;
using ExcelWindowPtrT       = std::unique_ptr<ExcelWindow>;
using PositionPtrT          = std::unique_ptr<Position>;
using MarketWatchPtrT       = std::unique_ptr<MarketWatch>;
using OrderFormPtrT         = std::shared_ptr<OrderForm>;
using MulticastReceiverPtrT = std::unique_ptr<MulticastReceiver>;
using OpenOrdersPtrT        = std::unique_ptr<OpenOrders>;
using StrategyWorkspacePtrT = std::unique_ptr<StrategyWorkspace>;
using TradeHistoryPtrT      = std::unique_ptr<TradeHistory>;
using MessageBrokerPtrT     = std::unique_ptr<MessageBroker>;
using OptionChainPtrT       = std::unique_ptr<OptionChain>;
using OrderBookPtrT         = std::unique_ptr<OrderBook>;

using ThreadPtrT   = std::unique_ptr<std::jthread>;
using ThreadGroupT = std::vector<ThreadPtrT>;

using TimerT          = boost::asio::deadline_timer;
using ExecutorT       = boost::asio::io_context;
using ExecutorTypeT   = ExecutorT::executor_type;
using ExecutorStrandT = ExecutorT::strand;
using ErrorCodeT      = boost::system::error_code;
using WorkerT         = boost::asio::executor_work_guard<ExecutorTypeT>;

using CancelPendingOrderFunctionT = std::function<void(OrderInfoPtrT)>;
