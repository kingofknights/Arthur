//
// Created by VIKLOD on 21-01-2023.
//

#include "../include/Arthur.hpp"

#include <Psapi.h>

#include <algorithm>
#include <nlohmann/json.hpp>
#include <thread>

#include "../API/ContractInfo.hpp"
#include "../API/TokenInfo.hpp"
#include "../Audio/Sound.hpp"
#include "../DataFeed/Compression.h"
#include "../Demo/Demo.hpp"
#include "../Knight/Scanner.hpp"
#include "../include/Colors.hpp"
#include "../include/ColumnGenerator.hpp"
#include "../include/ConfigLoader.hpp"
#include "../include/Enums.hpp"
#include "../include/GreekBook.hpp"
#include "../include/ManualOrder.hpp"
#include "../include/MarketWatch.hpp"
#include "../include/MessageBroker.hpp"
#include "../include/Multicast.hpp"
#include "../include/OptionChain.hpp"
#include "../include/OrderBook.hpp"
#include "../include/PendingBook.hpp"
#include "../include/Portfolio.hpp"
#include "../include/StrategyWorkspace.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Themes.hpp"
#include "../include/TradeBook.hpp"
#include "../include/Utils.hpp"
#include "../include/plf_nanotimer.h"

class MemoryUsage {
public:
	static double GetRamUsage();
};

extern bool					BackendConnected;
extern int					UserID;
extern DemoOrderInfoSignalT DemoOrderInfoSignal;
extern MarketEventQueueT	MarketEventQueue;
extern AllContractT			AllContract;

#define DATABASE_PATH "ResultSet.db3"
#define TRADING_APP_CONFIG_PATH "Config/Arthur.json"
#define ORDER_ALL_BOOK "Order All Book"
#define REJECT_BOOK "Reject Book"

Arthur::Arthur(bool* closeMainWindow_) : _closeMainWindow(closeMainWindow_), _backendWorker(_backendComService.get_executor()), _backendStrand(_backendComService) {
	Themes::AddIconFonts("Ruda-Bold.ttf", 18.0f);
#if 1
	UserID	   = 101;
	_ipaddress = "127.0.0.1";
	_port	   = "54321";

	LOG(INFO, "Loading SqlLite3 Database : {}", DATABASE_PATH)
	Lancelot::ContractInfo::Initialize(DATABASE_PATH, Utils::GetAllContractCallback);
	std::sort(AllContract.begin(), AllContract.end(), std::less<>());
	Utils::GetClientList(UserID);
	Utils::CreateSupportFolder();
	ConfigLoader::Instance();

	_demoPtr			  = std::make_unique<Demo>();
	_columnGeneratorPtr	  = std::make_unique<ColumnGenerator>();
	_greekBookPtr		  = std::make_unique<GreekBook>(_backendComService);
	_manualOrderPtr		  = std::make_shared<ManualOrder>(_backendStrand);
	_marketWatchPtr		  = std::make_unique<MarketWatch>(_manualOrderPtr);
	_pendingBook		  = std::make_unique<PendingBook>(_manualOrderPtr, _backendStrand);
	_strategyWorkspacePtr = std::make_unique<StrategyWorkspace>(_backendStrand);
	_tradeBookPtr		  = std::make_unique<TradeBook>();
	_optionChainPtr		  = std::make_unique<OptionChain>();
	_messageBroker		  = std::make_unique<MessageBroker>(_backendComService);
	_multicastReceiverPtr = std::make_unique<MulticastReceiver>(_backendComService);
	_orderBookPtr		  = std::make_unique<OrderBook>(ORDER_ALL_BOOK);
	_rejectBookPtr		  = std::make_unique<OrderBook>(REJECT_BOOK);
	_tradeSoundPtr		  = std::make_unique<Sound>("collide.wav");

	imports(TRADING_APP_CONFIG_PATH);
	SetTheme(static_cast<VisualTheme>(_theme));
	{
		auto callback = [&](const StrategyRowPtrT& row_, const std::string& name_, RequestType type_) { strategyRequestEvent(row_, name_, type_); };
		PortfolioInterface::setStrategyActionCallback(std::move(callback));
	}
	{
		auto callback = [&](const std::string& contract_) { _optionChainPtr->SetOptionForFuture(contract_); };
		_marketWatchPtr->Connect(std::move(callback));
	}
	{
		auto callback = [&](const std::string& contract_) { _marketWatchPtr->addContractToMarketWatch(contract_); };
		Portfolio::setCallback(std::move(callback));
	}
	{
		auto callback = [&](const ManualOrderInfoT& ManualOrderInfo_, RequestType type_) { manualOrderRequestEvent(ManualOrderInfo_, type_); };
		_manualOrderPtr->publishOrderCallback(std::move(callback));
	}
	{
		auto callback = [&](const OrderInfoPtrT& orderInfo_) { cancelOrderEvent(orderInfo_); };
		_pendingBook->cancelOrderFunctionCallback(std::move(callback));
	}
	{
		auto callback = [&](const OrderInfoPtrT& orderInfo_) { AddTrade(orderInfo_); };
		_messageBroker->setCallback(std::move(callback));
	}

	startAllThreads();
#endif
}

Arthur::~Arthur() {
	exports(TRADING_APP_CONFIG_PATH);
	LOG(INFO, "{}", __FUNCTION__)
	_backendWorker.reset();
	std::ranges::for_each(_threadGroup, [](std::unique_ptr<std::jthread>& thread_) {
		auto id = thread_->native_handle();
		LOG(INFO, "{} Requesting Stop", id)
		thread_->request_stop();
		LOG(INFO, "{} Thread Requested Stop", id)
	});
	plf::nanotimer timer;
	try {
		LOG(INFO, "{}", "boost::asio::io_service : stopping")
		timer.start();
		_backendComService.stop();
		LOG(INFO, "{} {}", "boost::asio::io_service : stopped", timer.get_elapsed_ns())

		LOG(INFO, "{}", "Column Generator : stopping")
		timer.start();
		_columnGeneratorPtr.reset();
		LOG(INFO, "{} {}", "Column Generator : stopped", timer.get_elapsed_ns())
		LOG(INFO, "{}", "Option Chain : stopping")
		timer.start();
		_optionChainPtr.reset();
		LOG(INFO, "{} {}", "Option Chain : stopped", timer.get_elapsed_ns())
		LOG(INFO, "{}", "Pending Book : stopping")
		timer.start();
		_pendingBook.reset();
		LOG(INFO, "{} {}", "Pending Book : stopped", timer.get_elapsed_ns())
		LOG(INFO, "{}", "Trade Book : stopping")
		timer.start();
		_tradeBookPtr.reset();
		LOG(INFO, "{} {}", "Trade Book : stopped", timer.get_elapsed_ns())
		LOG(INFO, "{}", "Greeks Book : stopping")
		timer.start();
		_greekBookPtr.reset();
		LOG(INFO, "{} {}", "Greeks Book : stopped", timer.get_elapsed_ns())
		LOG(INFO, "{}", "Market Watch : stopping")
		timer.start();
		_marketWatchPtr.reset();
		LOG(INFO, "{} {}", "Market Watch : stopped", timer.get_elapsed_ns())
		LOG(INFO, "{}", "Strategy Workspace : stopping")
		timer.start();
		_strategyWorkspacePtr.reset();
		LOG(INFO, "{} {}", "Strategy Workspace : stopped", timer.get_elapsed_ns())

		LOG(INFO, "{}", "Excel Automation : stopping")
		LOG(INFO, "{}", "Excel Automation : stopped")

		LOG(INFO, "{}", "Demo Thread : stopping")
		timer.start();
		_demoPtr.reset();
		LOG(INFO, "{} {}", "Demo Thread : stopped", timer.get_elapsed_ns())

		LOG(INFO, "{}", "Manual Order : stopping")
		timer.start();
		_manualOrderPtr.reset();
		LOG(INFO, "{} {}", "Manual Order : stopped", timer.get_elapsed_ns())

		LOG(INFO, "{}", "Multicast Receiver : stopping")
		timer.start();
		_multicastReceiverPtr.reset();
		LOG(INFO, "{} {}", "Multicast Receiver : stopped", timer.get_elapsed_ns())
	} catch (std::exception& exception_) {
		LOG(ERROR, "{} {}", "exception thrown", exception_.what())
	} catch (...) {
		LOG(ERROR, "{}", "exception thrown : unkouwn")
	}
}

void Arthur::paint() {
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

#if 1
	Menu();
	if (_showDemoWindow) {
		ImGui::ShowDemoWindow(&_showDemoWindow);
	}

	_greekBookPtr->paint(&_showGreekBooks);
	_marketWatchPtr->paint(&_showMarketWatch, &_showMarketLadder);
	_pendingBook->paint(&_showPendingBook);
	_strategyWorkspacePtr->paint(&_showStrategyWorkspace);
	_tradeBookPtr->paint(&_showTradeBook);
	_optionChainPtr->paint(&_showOptionChain);
	_orderBookPtr->paint(&_showOrderBook);
	_rejectBookPtr->paint(&_showRejectBook);
	Utils::StatusBar();
#endif
}

void Arthur::AddTrade(const OrderInfoPtrT& tradeInfo_) {
	LOG(INFO, "{} {} {} {} {}", __FUNCTION__, tradeInfo_->Gateway, tradeInfo_->Token, tradeInfo_->Side, tradeInfo_->StatusValue)
	switch (tradeInfo_->StatusValue) {
		case OrderStatus_PLACED:
		case OrderStatus_NEW:
		case OrderStatus_REPLACED: {
			_pendingBook->Insert(tradeInfo_, true);
			break;
		}
		case OrderStatus_CANCELLED: {
			_pendingBook->Insert(tradeInfo_, false);
			break;
		}
		case OrderStatus_FILLED:
		case OrderStatus_PARTIAL_FILLED: {
			_pendingBook->Insert(tradeInfo_, false);
			_tradeBookPtr->Insert(tradeInfo_);
			_greekBookPtr->Insert(tradeInfo_);
			_tradeSoundPtr->Play();
			break;
		}
		case OrderStatus_NEW_REJECT:
		case OrderStatus_REPLACE_REJECT:
		case OrderStatus_CANCEL_REJECT: {
			_rejectBookPtr->Insert(tradeInfo_);
			break;
		}
	}
	_orderBookPtr->Insert(tradeInfo_);
}

auto Arthur::Menu() -> void {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu(ICON_MD_MENU " File")) {
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_MD_EXIT_TO_APP " Exit")) {
				*_closeMainWindow = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(ICON_MD_DESKTOP_WINDOWS " View")) {
			if (ImGui::Checkbox("Column Generator", &_showColumnGenerator)) {
				ImGui::OpenPopup(COLUMN_GENERATOR_WINDOW);
			}
			if (_showColumnGenerator) _columnGeneratorPtr->paint(&_showColumnGenerator);

			Utils::ToggleMenuItem("Market Watch", _showMarketWatch);
			Utils::ToggleMenuItem("Market Ladder", _showMarketLadder);
			Utils::ToggleMenuItem("Strategy Workspace", _showStrategyWorkspace);

			if (ImGui::BeginMenu(ICON_MD_LIBRARY_BOOKS " Book")) {
				Utils::ToggleMenuItem("Trade Book", _showTradeBook);
				Utils::ToggleMenuItem("Pending Book", _showPendingBook);
				Utils::ToggleMenuItem("Greeks Book", _showGreekBooks);
				Utils::ToggleMenuItem("Order Book", _showOrderBook);
				Utils::ToggleMenuItem("Reject Book", _showRejectBook);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(ICON_MD_CONSTRUCTION " Tools ")) {
			if (strcmp(_password, "Password") == 0) {
				if (ImGui::MenuItem(ICON_MD_DEVELOPER_MODE " Demo")) {
					_showDemoWindow = not _showDemoWindow;
					if (not _showDemoWindow) {
						std::memset(_password, '\0', 10);
					}
				}
			} else {
				ImGui::InputText("Password", _password, 10, ImGuiInputTextFlags_Password);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(ICON_MD_DISPLAY_SETTINGS " Themes")) {
			if (ImGui::BeginCombo("Theme", VisualThemeName[_theme])) {
				for (int i = 0; i < VisualTheme_END; ++i) {
					if (ImGui::Selectable(VisualThemeName[i], _theme == i)) {
						SetTheme(static_cast<VisualTheme>(i));
					}
				}
				ImGui::EndCombo();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(ICON_MD_PAID " Paid")) {
			Utils::ToggleMenuItem("Option Chain", _showOptionChain);
			// Utils::ToggleMenuItem("Excel Automation", _showExcelWindow);
			ImGui::EndMenu();
		}

		ImGui::Text(ICON_MD_AUTOFPS_SELECT " FPS : %.0f", ImGui::GetIO().Framerate);
		ImGui::SameLine();
		ImGui::Text(ICON_MD_MEMORY " RAM : %.2f", MemoryUsage::GetRamUsage());
		ImGui::SameLine();
		ImGui::TextColored(UpDownColor(BackendConnected), "%s%s:%s", ICON_MD_LAN, _ipaddress.data(), _port.data());
		ImGui::SameLine();
		static bool start_demo = true;
		if (start_demo and ImGui::Button(ICON_MD_PLAY_ARROW " Play Demo")) {
			_demoPtr->startAndStop();
			start_demo = false;
		}

		ImGui::EndMainMenuBar();
	}
}

auto Arthur::run(std::stop_token& stopToken_) -> void {
	while (not stopToken_.stop_requested()) {
		boost::system::error_code errorCode;
		_backendComService.run(errorCode);
		LOG(WARNING, "boost::asio::io_service {}", errorCode.message())
	}
	LOG(WARNING, "{} {}", __FUNCTION__, "Exiting")
}

void Arthur::SetTheme(VisualTheme theme_) {
	switch (theme_) {
		case VisualTheme_MATERIAL_LIGHT: {
			Themes::ImGuiMaterialLightStyle();
			break;
		}
		case VisualTheme_MATERIAL_DARK: {
			Themes::ImGuiMaterialDarkStyle();
			break;
		}
		case VisualTheme_ONE_LIGHT: {
			Themes::ImGuiOneLightStyle();
			break;
		}
		case VisualTheme_ONE_DARK: {
			Themes::ImGuiOneDarkStyle();
			break;
		}
		case VisualTheme_ARTHUR_LIGHT: {
			Themes::ImGuiArthurLightStyle();
			break;
		}
		case VisualTheme_ARTHUR_DARK: {
			Themes::ImGuiArthurDarkStyle();
			break;
		}
		case VisualTheme_IMGUI_LIGHT: {
			ImGui::StyleColorsLight();
			break;
		}
		case VisualTheme_IMGUI_DARK: {
			ImGui::StyleColorsDark();
			break;
		}
	}
	_theme = theme_;
}

auto Arthur::imports(std::string_view path_) -> void {
	std::fstream file(path_.data(), std::ios::in);
	if (not file.is_open()) return;

	nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);

	_showDemoWindow		   = root[Configuration[ConfigFile_DEMO]].get<bool>();
	_showExcelWindow	   = root[Configuration[ConfigFile_EXCEL_WINDOW]].get<bool>();
	_showGreekBooks		   = root[Configuration[ConfigFile_GREEK_BOOK]].get<bool>();
	_theme				   = root[Configuration[ConfigFile_THEME]].get<int>();
	_showMarketLadder	   = root[Configuration[ConfigFile_MARKET_LADDER]].get<bool>();
	_showMarketWatch	   = root[Configuration[ConfigFile_MARKET_WATCH]].get<bool>();
	_showPendingBook	   = root[Configuration[ConfigFile_PENDING_BOOK]].get<bool>();
	_showStrategyWorkspace = root[Configuration[ConfigFile_STRATEGY_WORKSPACE]].get<bool>();
	_showTradeBook		   = root[Configuration[ConfigFile_TRADE_BOOK]].get<bool>();
	_showOrderBook		   = root[Configuration[ConfigFile_ORDER_ALL_BOOK]].get<bool>();
	_showRejectBook		   = root[Configuration[ConfigFile_REJECT_BOOK]].get<bool>();
	_showOptionChain	   = root[Configuration[ConfigFile_OPTION_CHAIN]].get<bool>();

	LOG(INFO, "Import Config File {}", path_)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_DEMO], _showDemoWindow)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_EXCEL_WINDOW], _showExcelWindow)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_GREEK_BOOK], _showGreekBooks)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_THEME], _theme)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_MARKET_LADDER], _showMarketLadder)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_MARKET_WATCH], _showMarketWatch)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_PENDING_BOOK], _showPendingBook)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_STRATEGY_WORKSPACE], _showStrategyWorkspace)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_TRADE_BOOK], _showTradeBook)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_ORDER_ALL_BOOK], _showOrderBook)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_REJECT_BOOK], _showRejectBook)
	LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_OPTION_CHAIN], _showOptionChain)

	file.close();
}

auto Arthur::exports(std::string_view path_) -> void {
	nlohmann::ordered_json root;
	root[Configuration[ConfigFile_DEMO]]			   = _showDemoWindow;
	root[Configuration[ConfigFile_EXCEL_WINDOW]]	   = _showExcelWindow;
	root[Configuration[ConfigFile_GREEK_BOOK]]		   = _showGreekBooks;
	root[Configuration[ConfigFile_THEME]]			   = _theme;
	root[Configuration[ConfigFile_MARKET_LADDER]]	   = _showMarketLadder;
	root[Configuration[ConfigFile_MARKET_WATCH]]	   = _showMarketWatch;
	root[Configuration[ConfigFile_PENDING_BOOK]]	   = _showPendingBook;
	root[Configuration[ConfigFile_STRATEGY_WORKSPACE]] = _showStrategyWorkspace;
	root[Configuration[ConfigFile_TRADE_BOOK]]		   = _showTradeBook;
	root[Configuration[ConfigFile_ORDER_ALL_BOOK]]	   = _showOrderBook;
	root[Configuration[ConfigFile_REJECT_BOOK]]		   = _showRejectBook;
	root[Configuration[ConfigFile_OPTION_CHAIN]]	   = _showOptionChain;

	std::fstream file(path_.data(), std::ios::trunc | std::ios::out);
	if (not file.is_open()) return;

	file << root.dump();
	file.close();

	LOG(INFO, "Export Config File {}", path_)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_DEMO], _showDemoWindow)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_EXCEL_WINDOW], _showExcelWindow)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_GREEK_BOOK], _showGreekBooks)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_THEME], _theme)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_MARKET_LADDER], _showMarketLadder)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_MARKET_WATCH], _showMarketWatch)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_PENDING_BOOK], _showPendingBook)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_STRATEGY_WORKSPACE], _showStrategyWorkspace)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_TRADE_BOOK], _showTradeBook)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_ORDER_ALL_BOOK], _showOrderBook)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_REJECT_BOOK], _showRejectBook)
	LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_OPTION_CHAIN], _showOptionChain)
}

void Arthur::startAllThreads() {
	{
		auto thread = std::make_unique<std::jthread>([&](std::stop_token token_) { run(token_); });
		_threadGroup.push_back(std::move(thread));
	}

	{
		DemoOrderInfoSignal.connect([&](const OrderInfoPtrT& orderInfo_) { AddTrade(orderInfo_); });
		auto thread = std::make_unique<std::jthread>([&](std::stop_token token_) { _demoPtr->Run(token_); });
		_threadGroup.push_back(std::move(thread));
	}

	{
		auto thread = std::make_unique<std::jthread>([&](std::stop_token token_) { marketEventHandler(token_); });
		_threadGroup.push_back(std::move(thread));
	}
	//{ _messageBroker->makeConnection(_ipaddress, _port); }
}

void Arthur::marketEventHandler(std::stop_token& stopToken_) {
	while (not stopToken_.stop_requested()) {
		MarketEventQueue.consume_one([&](const MarketWatchDataPtrT& pointer_) { Scanner::GetInstance().Process(pointer_->Token); });
	}
	LOG(WARNING, "{} {}", __FUNCTION__, "Exiting")
}

double MemoryUsage::GetRamUsage() {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(PROCESS_MEMORY_COUNTERS_EX));
	return (double)pmc.WorkingSetSize / (1024 * 1024);
}

void Arthur::manualOrderRequestEvent(const ManualOrderInfoT& ManualOrderInfo, RequestType type_) {
	std::string	   config		 = Utils::manualSerialize(ManualOrderInfo);
	RequestInPackT requestInPack = Compression::CompressData(config, UserID, type_);
	if (_messageBroker) {
		_messageBroker->Write_Async((char*)&requestInPack, sizeof(RequestInPackT));
	}
	LOG(WARNING, "{} {} {}", config, type_, requestInPack.Type)
}

void Arthur::strategyRequestEvent(StrategyRowPtrT row_, const std::string& name_, RequestType type_) {
	std::string	   config		 = Utils::strategySerialize(row_, name_, type_);
	RequestInPackT requestInPack = Compression::CompressData(config, UserID, type_);
	LOG(WARNING, "{} {}", config, type_)
	if (_messageBroker) {
		_messageBroker->Write_Async((char*)&requestInPack, sizeof(RequestInPackT));
	}
}

void Arthur::cancelOrderEvent(const OrderInfoPtrT& orderInfo_) {
	std::string	   config		 = Utils::cancelOrderSerialize(orderInfo_);
	RequestInPackT requestInPack = Compression::CompressData(config, UserID, RequestType_CANCEL);
	LOG(WARNING, "{} {}", config, RequestType_CANCEL)
	if (_messageBroker) {
		_messageBroker->Write_Async((char*)&requestInPack, sizeof(RequestInPackT));
	}
}
