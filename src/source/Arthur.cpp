//
// Created by VIKLOD on 21-01-2023.
//

#include "../include/Arthur.hpp"
#include "Lancelot/Structure.hpp"

#if _WIN32
#include <Psapi.h>
#endif

#include <algorithm>
#include <nlohmann/json.hpp>
#include <thread>

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../API/TokenInfo.hpp"
#include "../Knight/Scanner.hpp"
#include "../include/Colors.hpp"
#include "../include/ConfigLoader.hpp"
#include "../include/Enums.hpp"
#include "../include/MarketWatch.hpp"
#include "../include/MessageBroker.hpp"
#include "../include/Multicast.hpp"
#include "../include/OpenOrders.hpp"
#include "../include/OptionChain.hpp"
#include "../include/OrderBook.hpp"
#include "../include/OrderForm.hpp"
#include "../include/Portfolio.hpp"
#include "../include/Position.hpp"
#include "../include/StrategyWorkspace.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/TemplateBuilder.hpp"
#include "../include/Themes.hpp"
#include "../include/TradeHistory.hpp"
#include "../include/Utils.hpp"
#include "../include/plf_nanotimer.h"

class MemoryUsage {
  public:
    static auto GetRamUsage() -> double;
};

extern bool                 BackendConnected;
extern int                  UserID;
extern DemoOrderInfoSignalT DemoOrderInfoSignal;
extern MarketEventQueueT    MarketEventQueue;
extern AllContractT         AllContract;

#define DATABASE_PATH "/home/vikram.lodhi@corp.merillife.com/Projects/historical/20240912/fo_ref_contract_master.20240912.csv"
#define TRADING_APP_CONFIG_PATH "Config/Arthur.json"
#define ORDER_ALL_BOOK "Order All Book"
#define REJECT_BOOK "Reject Book"

Arthur::Arthur(bool* closeMainWindow_) : _backendStrand(_backendComService), _backendWorker(_backendComService.get_executor()), _closeMainWindow(closeMainWindow_) {
    Themes::AddIconFonts("Ruda-Bold.ttf", 18.0f);
    UserID     = 101;
    _ipaddress = "127.0.0.1";
    _port      = "9898";

    LOG(INFO, "Loading SqlLite3 Database : {}", DATABASE_PATH)
    Lancelot::ContractInfo::Initialize(DATABASE_PATH, Utils::GetAllContractCallback);
    std::ranges::sort(AllContract, std::less<>());
    Utils::GetClientList(UserID);
    Utils::CreateSupportFolder();
    ConfigLoader::Instance();

    _templateBuilderPtr   = std::make_unique<TemplateBuilder>();
    _positionPtr          = std::make_unique<Position>(_backendComService);
    _orderFormPtr         = std::make_shared<OrderForm>(_backendStrand);
    _marketWatchPtr       = std::make_unique<MarketWatch>(_orderFormPtr);
    _openOrdersPtr        = std::make_unique<OpenOrders>(_orderFormPtr, _backendStrand);
    _strategyWorkspacePtr = std::make_unique<StrategyWorkspace>(_backendStrand);
    _tradeHistoryPtr      = std::make_unique<TradeHistory>();
    _optionChainPtr       = std::make_unique<OptionChain>();
    _messageBroker        = std::make_unique<MessageBroker>(_backendComService);
    _multicastReceiverPtr = std::make_unique<MulticastReceiver>(_backendComService);
    _orderBookPtr         = std::make_unique<OrderBook>(ORDER_ALL_BOOK);
    _rejectBookPtr        = std::make_unique<OrderBook>(REJECT_BOOK);

    imports(TRADING_APP_CONFIG_PATH);
    SetTheme(static_cast<VisualTheme>(_theme));
    {
        auto callback = [&](const StrategyRowPtrT& row_, const std::string& name_, Lancelot::RequestType type_) { strategyRequestEvent(row_, name_, type_); };
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
        auto callback = [&](const OrderFormInfoT& ManualOrderInfo_, Lancelot::RequestType type_) { manualOrderRequestEvent(ManualOrderInfo_, type_); };
        _orderFormPtr->publishOrderCallback(std::move(callback));
    }
    {
        auto callback = [&](const OrderInfoPtrT& orderInfo_) { cancelOrderEvent(orderInfo_); };
        _openOrdersPtr->cancelOrderFunctionCallback(std::move(callback));
    }
    {
        auto callback = [&](const OrderInfoPtrT& orderInfo_) { AddTrade(orderInfo_); };
        _messageBroker->setCallback(std::move(callback));
    }

    startAllThreads();
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
        _templateBuilderPtr.reset();
        LOG(INFO, "{} {}", "Column Generator : stopped", timer.get_elapsed_ns())
        LOG(INFO, "{}", "Option Chain : stopping")
        timer.start();
        _optionChainPtr.reset();
        LOG(INFO, "{} {}", "Option Chain : stopped", timer.get_elapsed_ns())
        LOG(INFO, "{}", "Pending Book : stopping")
        timer.start();
        _openOrdersPtr.reset();
        LOG(INFO, "{} {}", "Pending Book : stopped", timer.get_elapsed_ns())
        LOG(INFO, "{}", "Trade Book : stopping")
        timer.start();
        _tradeHistoryPtr.reset();
        LOG(INFO, "{} {}", "Trade Book : stopped", timer.get_elapsed_ns())
        LOG(INFO, "{}", "Greeks Book : stopping")
        timer.start();
        _positionPtr.reset();
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

        LOG(INFO, "{}", "Manual Order : stopping")
        timer.start();
        _orderFormPtr.reset();
        LOG(INFO, "{} {}", "Manual Order : stopped", timer.get_elapsed_ns())

        _orderBookPtr.reset();
        _rejectBookPtr.reset();

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
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    Menu();
    if (_showDemoWindow) {
        ImGui::ShowDemoWindow(&_showDemoWindow);
    }

    _positionPtr->paint(&_showPosition);
    _marketWatchPtr->paint(&_showMarketWatch, &_showPriceLadder);
    _openOrdersPtr->paint(&_showOpenOrders);
    _strategyWorkspacePtr->paint(&_showStrategyWorkspace);
    _tradeHistoryPtr->paint(&_showTradeHistory);
    _optionChainPtr->paint(&_showOptionChain);
    _orderBookPtr->paint(&_showOrderBook);
    _rejectBookPtr->paint(&_showRejectBook);
    Utils::StatusBar();
}

void Arthur::AddTrade(const OrderInfoPtrT& tradeInfo_) {
    LOG(INFO, "{} {}", __PRETTY_FUNCTION__, static_cast<int>(tradeInfo_->StatusValue));
    switch (tradeInfo_->StatusValue) {
        case OrderStatus_PLACED:
        case OrderStatus_NEW:
        case OrderStatus_REPLACED: {
            _openOrdersPtr->Insert(tradeInfo_, true);
            break;
        }
        case OrderStatus_CANCELLED: {
            _openOrdersPtr->Insert(tradeInfo_, false);
            break;
        }
        case OrderStatus_FILLED:
        case OrderStatus_PARTIAL_FILLED: {
            _openOrdersPtr->Insert(tradeInfo_, false);
            _tradeHistoryPtr->Insert(tradeInfo_);
            _positionPtr->Insert(tradeInfo_);
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
            if (ImGui::Checkbox("Template Builder", &_showTemplateBuilder)) {
                ImGui::OpenPopup(COLUMN_GENERATOR_WINDOW);
            }
            if (_showTemplateBuilder) _templateBuilderPtr->paint(&_showTemplateBuilder);

            Utils::ToggleMenuItem("Market Watch", _showMarketWatch);
            Utils::ToggleMenuItem("Price Ladder", _showPriceLadder);
            Utils::ToggleMenuItem("Strategy Workspace", _showStrategyWorkspace);

            if (ImGui::BeginMenu(ICON_MD_LIBRARY_BOOKS " Book")) {
                Utils::ToggleMenuItem("Trade History", _showTradeHistory);
                Utils::ToggleMenuItem("Open Orders", _showOpenOrders);
                Utils::ToggleMenuItem("Position", _showPosition);
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
        case VisualTheme_END:
            break;
    }
    _theme = theme_;
}

auto Arthur::imports(std::string_view path_) -> void {
    std::fstream file(path_.data(), std::ios::in);
    if (not file.is_open()) {
        return;
    }

    nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);

    _showDemoWindow        = root[Configuration[ConfigFile_DEMO]].get<bool>();
    _showExcelWindow       = root[Configuration[ConfigFile_EXCEL_WINDOW]].get<bool>();
    _showPosition          = root[Configuration[ConfigFile_GREEK_BOOK]].get<bool>();
    _theme                 = root[Configuration[ConfigFile_THEME]].get<int>();
    _showPriceLadder       = root[Configuration[ConfigFile_MARKET_LADDER]].get<bool>();
    _showMarketWatch       = root[Configuration[ConfigFile_MARKET_WATCH]].get<bool>();
    _showOpenOrders        = root[Configuration[ConfigFile_PENDING_BOOK]].get<bool>();
    _showStrategyWorkspace = root[Configuration[ConfigFile_STRATEGY_WORKSPACE]].get<bool>();
    _showTradeHistory      = root[Configuration[ConfigFile_TRADE_BOOK]].get<bool>();
    _showOrderBook         = root[Configuration[ConfigFile_ORDER_ALL_BOOK]].get<bool>();
    _showRejectBook        = root[Configuration[ConfigFile_REJECT_BOOK]].get<bool>();
    _showOptionChain       = root[Configuration[ConfigFile_OPTION_CHAIN]].get<bool>();

    LOG(INFO, "Import Config File {}", path_)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_DEMO], _showDemoWindow)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_EXCEL_WINDOW], _showExcelWindow)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_GREEK_BOOK], _showPosition)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_THEME], _theme)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_MARKET_LADDER], _showPriceLadder)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_MARKET_WATCH], _showMarketWatch)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_PENDING_BOOK], _showOpenOrders)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_STRATEGY_WORKSPACE], _showStrategyWorkspace)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_TRADE_BOOK], _showTradeHistory)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_ORDER_ALL_BOOK], _showOrderBook)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_REJECT_BOOK], _showRejectBook)
    LOG(INFO, "Reading {} [{}]", Configuration[ConfigFile_OPTION_CHAIN], _showOptionChain)

    file.close();
}

auto Arthur::exports(std::string_view path_) -> void {
    nlohmann::ordered_json root;
    root[Configuration[ConfigFile_DEMO]]               = _showDemoWindow;
    root[Configuration[ConfigFile_EXCEL_WINDOW]]       = _showExcelWindow;
    root[Configuration[ConfigFile_GREEK_BOOK]]         = _showPosition;
    root[Configuration[ConfigFile_THEME]]              = _theme;
    root[Configuration[ConfigFile_MARKET_LADDER]]      = _showPriceLadder;
    root[Configuration[ConfigFile_MARKET_WATCH]]       = _showMarketWatch;
    root[Configuration[ConfigFile_PENDING_BOOK]]       = _showOpenOrders;
    root[Configuration[ConfigFile_STRATEGY_WORKSPACE]] = _showStrategyWorkspace;
    root[Configuration[ConfigFile_TRADE_BOOK]]         = _showTradeHistory;
    root[Configuration[ConfigFile_ORDER_ALL_BOOK]]     = _showOrderBook;
    root[Configuration[ConfigFile_REJECT_BOOK]]        = _showRejectBook;
    root[Configuration[ConfigFile_OPTION_CHAIN]]       = _showOptionChain;

    std::fstream file(path_.data(), std::ios::trunc | std::ios::out);
    if (not file.is_open()) return;

    file << root.dump();
    file.close();

    LOG(INFO, "Export Config File {}", path_)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_DEMO], _showDemoWindow)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_EXCEL_WINDOW], _showExcelWindow)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_GREEK_BOOK], _showPosition)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_THEME], _theme)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_MARKET_LADDER], _showPriceLadder)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_MARKET_WATCH], _showMarketWatch)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_PENDING_BOOK], _showOpenOrders)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_STRATEGY_WORKSPACE], _showStrategyWorkspace)
    LOG(INFO, "Saving {} [{}]", Configuration[ConfigFile_TRADE_BOOK], _showTradeHistory)
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
        auto thread = std::make_unique<std::jthread>([&](std::stop_token token_) { marketEventHandler(token_); });
        _threadGroup.push_back(std::move(thread));
    }
    _multicastReceiverPtr->bindMC("127.0.0.1", 1223);
    _multicastReceiverPtr->read();

    { _messageBroker->makeConnection(_ipaddress, _port); }
}

void Arthur::marketEventHandler(std::stop_token& stopToken_) {
    while (not stopToken_.stop_requested()) {
        MarketEventQueue.consume_one([&](const MarketWatchDataPtrT& pointer_) { Scanner::GetInstance().Process(pointer_->Token); });
    }
    LOG(WARNING, "{} {}", __FUNCTION__, "Exiting")
}

double MemoryUsage::GetRamUsage() {
#if _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(PROCESS_MEMORY_COUNTERS_EX));
    return (double)pmc.WorkingSetSize / (1024 * 1024);
#else
    return 0.0;
#endif
}

void Arthur::manualOrderRequestEvent(const OrderFormInfoT& ManualOrderInfo, Lancelot::RequestType type_) {
    switch (type_) {
        case Lancelot::RequestType_LOGIN: {
            break;
        }
        case Lancelot::RequestType_NEW: {
            Lancelot::ManualOrder order{
                ._header = {
                    ._type   = 4001,
                    ._length = sizeof(Lancelot::ManualOrder) - sizeof(Lancelot::Header),
                },
                ._user = {
                    ._user      = static_cast<int16_t>(UserID),
                    ._portfolio = 9999,
                },
                ._token         = ManualOrderInfo.Self->Token,
                ._price         = static_cast<uint32_t>(ManualOrderInfo.Price * 100.0),
                ._quantity      = static_cast<uint32_t>(ManualOrderInfo.Quantity),
                ._triggerPrice  = 0,
                ._side          = ManualOrderInfo.Side,
                ._orderSequence = 0,
                ._orderType     = static_cast<int16_t>(ManualOrderInfo.Type),
                ._nnf           = 0,
            };
            _messageBroker->Write_Sync((char*)&order, sizeof(Lancelot::ManualOrder));
            break;
        }
        case Lancelot::RequestType_MODIFY: {
            Lancelot::ModifyOrder order{
                ._header = {
                    ._type   = 4010,
                    ._length = sizeof(Lancelot::ModifyOrder) - sizeof(Lancelot::Header),
                },
                ._user = {
                    ._user      = static_cast<int16_t>(UserID),
                    ._portfolio = 9999,
                },
                ._token         = ManualOrderInfo.Self->Token,
                ._orderSequence = ManualOrderInfo.Gateway,
                ._price         = static_cast<uint32_t>(ManualOrderInfo.Price * 100.0),
                ._quantity      = static_cast<uint32_t>(ManualOrderInfo.Quantity),
                ._triggerPrice  = 0,
            };
            _messageBroker->Write_Sync((char*)&order, sizeof(Lancelot::ModifyOrder));
            break;
        }
        case Lancelot::RequestType_CANCEL: {
            break;
        }
        case Lancelot::RequestType_APPLY:
        case Lancelot::RequestType_SUBSCRIBE:
        case Lancelot::RequestType_UNSUBSCRIBE:
        case Lancelot::RequestType_SUBSCRIBE_APPLY:
            break;
    }
}

void Arthur::strategyRequestEvent(StrategyRowPtrT row_, const std::string& name_, Lancelot::RequestType type_) {
}

void Arthur::cancelOrderEvent(const OrderInfoPtrT& orderInfo_) {
    Lancelot::CancelOrder order{
        ._header = {
            ._type   = 4011,
            ._length = sizeof(Lancelot::CancelOrder) - sizeof(Lancelot::Header),
        },
        ._user = {
            ._user      = static_cast<int16_t>(UserID),
            ._portfolio = 9999,
        },
        ._token         = orderInfo_->Token,
        ._orderSequence = orderInfo_->Gateway,

    };
    _messageBroker->Write_Sync((char*)&order, sizeof(Lancelot::CancelOrder));
}
