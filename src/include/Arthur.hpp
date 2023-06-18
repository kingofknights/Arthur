//
// Created by VIKLOD on 21-01-2023.
//

#ifndef ARTHUR_INCLUDE_ARTHUR_HPP
#define ARTHUR_INCLUDE_ARTHUR_HPP
#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string_view>
#include <thread>
#include <vector>

class ColumnGenerator;
class ExcelWindow;
class Demo;
class GreekBook;
class MarketWatch;
class ManualOrder;
class MulticastReceiver;
class PendingBook;
class StrategyWorkspace;
class TradeBook;
class MessageBroker;
class OptionChain;
class OrderBook;
class Sound;

struct OrderInfoT;
struct ManualOrderInfoT;
struct StrategyRowT;

using OrderInfoPtrT			= std::shared_ptr<OrderInfoT>;
using StrategyRowPtrT		= std::shared_ptr<StrategyRowT>;
using ColumnGeneratorPtrT	= std::unique_ptr<ColumnGenerator>;
using ExcelWindowPtrT		= std::unique_ptr<ExcelWindow>;
using GreekBookPtrT			= std::unique_ptr<GreekBook>;
using MarketWatchPtrT		= std::unique_ptr<MarketWatch>;
using ManualOrderPtrT		= std::shared_ptr<ManualOrder>;
using MulticastReceiverPtrT = std::unique_ptr<MulticastReceiver>;
using PendingBookPtrT		= std::unique_ptr<PendingBook>;
using StrategyWorkspacePtrT = std::unique_ptr<StrategyWorkspace>;
using TradeBookPtrT			= std::unique_ptr<TradeBook>;
using MessageBrokerPtrT		= std::unique_ptr<MessageBroker>;
using OptionChainPtrT		= std::unique_ptr<OptionChain>;
using DemoPtrT				= std::unique_ptr<Demo>;
using OrderBookPtrT			= std::unique_ptr<OrderBook>;
using SoundPtrT				= std::unique_ptr<Sound>;

using ThreadGroupT = std::vector<std::unique_ptr<std::jthread>>;
using TimerT	   = std::unique_ptr<boost::asio::deadline_timer>;

using ExecutorType = boost::asio::io_context::executor_type;
using WorkerT	   = boost::asio::executor_work_guard<ExecutorType>;

enum RequestType;
enum VisualTheme;

class Arthur {
public:
	explicit Arthur(bool *closeMainWindow_);

	~Arthur();
	void paint();
	void AddTrade(const OrderInfoPtrT &tradeInfo_);

protected:
	static void marketEventHandler(std::stop_token &token_);

	void SetTheme(VisualTheme theme_);
	void Menu();
	void run(std::stop_token &stopToken_);
	void imports(std::string_view path_);
	void exports(std::string_view path_);
	void startAllThreads();
	void manualOrderRequestEvent(const ManualOrderInfoT &ManualOrderInfo, RequestType type_);
	void strategyRequestEvent(StrategyRowPtrT row_, const std::string &name_, RequestType type_);
	void cancelOrderEvent(const OrderInfoPtrT &orderInfo_);

private:
	DemoPtrT			  _demoPtr				= nullptr;
	MessageBrokerPtrT	  _messageBroker		= nullptr;
	ColumnGeneratorPtrT	  _columnGeneratorPtr	= nullptr;
	GreekBookPtrT		  _greekBookPtr			= nullptr;
	ManualOrderPtrT		  _manualOrderPtr		= nullptr;
	MarketWatchPtrT		  _marketWatchPtr		= nullptr;
	MulticastReceiverPtrT _multicastReceiverPtr = nullptr;
	PendingBookPtrT		  _pendingBook			= nullptr;
	StrategyWorkspacePtrT _strategyWorkspacePtr = nullptr;
	TradeBookPtrT		  _tradeBookPtr			= nullptr;
	OptionChainPtrT		  _optionChainPtr		= nullptr;
	OrderBookPtrT		  _orderBookPtr			= nullptr;
	OrderBookPtrT		  _rejectBookPtr		= nullptr;
	SoundPtrT			  _tradeSoundPtr		= nullptr;

	bool _showColumnGenerator	= false;
	bool _showDemoWindow		= false;
	bool _showExcelWindow		= false;
	bool _showMarketLadder		= false;
	bool _showMarketWatch		= false;
	bool _showOptionChain		= false;
	bool _showPendingBook		= false;
	bool _showStrategyWorkspace = false;
	bool _showTradeBook			= false;
	bool _showGreekBooks		= false;
	bool _showOrderBook			= false;
	bool _showRejectBook		= false;
	int	 _theme					= 0;

	boost::asio::io_context			_backendComService;
	boost::asio::io_context::strand _backendStrand;
	WorkerT							_backendWorker;
	ThreadGroupT					_threadGroup;
	bool *							_closeMainWindow;
	char							_password[10]{};
	std::string						_ipaddress;
	std::string						_port;
};

#endif	// ARTHUR_INCLUDE_ARTHUR_HPP
