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

class TemplateBuilder;
class ExcelWindow;
class Demo;
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
class Sound;

struct OrderInfoT;
struct OrderFormInfoT;
struct StrategyRowT;

using OrderInfoPtrT			= std::shared_ptr<OrderInfoT>;
using StrategyRowPtrT		= std::shared_ptr<StrategyRowT>;
using TemplateBuilderPtrT	= std::unique_ptr<TemplateBuilder>;
using ExcelWindowPtrT		= std::unique_ptr<ExcelWindow>;
using PositionPtrT			= std::unique_ptr<Position>;
using MarketWatchPtrT		= std::unique_ptr<MarketWatch>;
using OrderFormPtrT			= std::shared_ptr<OrderForm>;
using MulticastReceiverPtrT = std::unique_ptr<MulticastReceiver>;
using OpenOrdersPtrT		= std::unique_ptr<OpenOrders>;
using StrategyWorkspacePtrT = std::unique_ptr<StrategyWorkspace>;
using TradeHistoryPtrT		= std::unique_ptr<TradeHistory>;
using MessageBrokerPtrT		= std::unique_ptr<MessageBroker>;
using OptionChainPtrT		= std::unique_ptr<OptionChain>;
using DemoPtrT				= std::unique_ptr<Demo>;
using OrderBookPtrT			= std::unique_ptr<OrderBook>;
using SoundPtrT				= std::unique_ptr<Sound>;

using ThreadGroupT = std::vector<std::unique_ptr<std::jthread>>;
using TimerT	   = std::unique_ptr<boost::asio::deadline_timer>;

using ExecutorType = boost::asio::io_context::executor_type;
using WorkerT	   = boost::asio::executor_work_guard<ExecutorType>;

enum RequestType : int;
enum VisualTheme : int;

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
	void manualOrderRequestEvent(const OrderFormInfoT &ManualOrderInfo, RequestType type_);
	void strategyRequestEvent(StrategyRowPtrT row_, const std::string &name_, RequestType type_);
	void cancelOrderEvent(const OrderInfoPtrT &orderInfo_);

private:
	DemoPtrT			  _demoPtr				;
	MessageBrokerPtrT	  _messageBroker		;
	TemplateBuilderPtrT	  _templateBuilderPtr	;
	PositionPtrT		  _positionPtr			;
	OrderFormPtrT		  _OrderFormPtr			;
	MarketWatchPtrT		  _marketWatchPtr		;
	MulticastReceiverPtrT _multicastReceiverPtr ;
	OpenOrdersPtrT		  _openOrdersPtr		;
	StrategyWorkspacePtrT _strategyWorkspacePtr ;
	TradeHistoryPtrT	  _tradeHistoryPtr		;
	OptionChainPtrT		  _optionChainPtr		;
	OrderBookPtrT		  _orderBookPtr			;
	OrderBookPtrT		  _rejectBookPtr		;
	SoundPtrT			  _tradeSoundPtr		;

	bool _showTemplateBuilder	= false;
	bool _showDemoWindow		= false;
	bool _showExcelWindow		= false;
	bool _showPriceLadder		= false;
	bool _showMarketWatch		= false;
	bool _showOptionChain		= false;
	bool _showOpenOrders		= false;
	bool _showStrategyWorkspace = false;
	bool _showTradeHistory		= false;
	bool _showPosition			= false;
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
