#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <deque>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#define TURNOFF_SCANNER
namespace Lancelot {
    enum Exchange : uint8_t;
    enum Instrument : uint8_t;
    enum OptionType : uint8_t;
    enum Side : uint8_t;
    enum RequestType : uint8_t;
    enum ResponseType : uint8_t;

    struct Header;
    struct HedgeOrderResponse;
}  // namespace Lancelot

class Arthur;
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
class Login;
class CentralFeed;
class TokenFilter;

struct OrderInfoT;
struct OrderFormInfoT;
struct StrategyRowT;
struct MarketWatchDataT;

using OrderInfoPtrT         = std::shared_ptr<OrderInfoT>;
using StrategyRowPtrT       = std::shared_ptr<StrategyRowT>;
using TemplateBuilderPtrT   = std::unique_ptr<TemplateBuilder>;
using ExcelWindowPtrT       = std::unique_ptr<ExcelWindow>;
using PositionPtrT          = std::unique_ptr<Position>;
using MarketWatchPtrT       = std::unique_ptr<MarketWatch>;
using OrderFormPtrT         = std::unique_ptr<OrderForm>;
using MulticastReceiverPtrT = std::unique_ptr<MulticastReceiver>;
using OpenOrdersPtrT        = std::unique_ptr<OpenOrders>;
using StrategyWorkspacePtrT = std::unique_ptr<StrategyWorkspace>;
using TradeHistoryPtrT      = std::unique_ptr<TradeHistory>;
using MessageBrokerPtrT     = std::unique_ptr<MessageBroker>;
using OptionChainPtrT       = std::unique_ptr<OptionChain>;
using OrderBookPtrT         = std::unique_ptr<OrderBook>;
using LoginPtrT             = std::unique_ptr<Login>;
using TokenFilterPtrT       = std::unique_ptr<TokenFilter>;

using MarketWatchDataPtrT = std::shared_ptr<MarketWatchDataT>;

using ThreadPtrT   = std::unique_ptr<std::jthread>;
using ThreadGroupT = std::vector<ThreadPtrT>;

using TimerT          = boost::asio::deadline_timer;
using ExecutorT       = boost::asio::io_context;
using ExecutorTypeT   = ExecutorT::executor_type;
using ExecutorStrandT = ExecutorT;
using ErrorCodeT      = boost::system::error_code;
using WorkerT         = boost::asio::executor_work_guard<ExecutorTypeT>;

using AddContractFunctionT = std::function<void(const std::string&)>;

using BookOrderListT = std::deque<OrderInfoPtrT>;
