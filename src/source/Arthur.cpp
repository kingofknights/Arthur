//
// Created by VIKLOD on 21-01-2023.
//

#include "../include/Arthur.hpp"

#include "Enums.hpp"
#include "Lancelot/Structure.hpp"
#include "Logger.hpp"
#include "include/Arthur_Fwd.hpp"
#include "include/PortfolioInterface.hpp"

#if _WIN32
#include <Psapi.h>
#endif

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

#include <nlohmann/json.hpp>

#include <algorithm>
#include <thread>

class MemoryUsage {
  public:
    static auto GetRamUsage() -> double;
};

extern AllContractT AllContract;

#define TRADING_APP_CONFIG_PATH "Config/Arthur.json"
#define ORDER_ALL_BOOK          "Order All Book"
#define REJECT_BOOK             "Reject Book"

Arthur::Arthur(bool* closeMainWindow_) : _strand(_executor), _backendWorker(_executor.get_executor()), _closeMainWindow(closeMainWindow_) {
    std::fstream file("setting.json");
    if (file.is_open()) {
        nlohmann::json        json        = nlohmann::json::parse(file);
        const nlohmann::json& font        = json["font"];
        const nlohmann::json& backend     = json["backend"];
        const nlohmann::json& marketwatch = json["marketwatch"];

        std::string fontFile = "Ruda-Bold.ttf";
        float       fontSize = 18.0F;
        font["file"].get_to(fontFile);
        font["size"].get_to(fontSize);

        backend["address"].get_to(_backend._address);
        backend["port"].get_to(_backend._port);
        backend["user"].get_to(_userId);

        std::string database;
        marketwatch["address"].get_to(_marketWatch._address);
        marketwatch["port"].get_to(_marketWatch._port);
        marketwatch["contract"].get_to(database);

        LOG(INFO, "Loading SqlLite3 Database : {}", database)
        Lancelot::ContractInfo::Initialize(database, Utils::GetAllContractCallback);

        Themes::AddIconFonts(fontFile, fontSize);
    } else {
        LOG(ERROR, "Config file not found : setting.json", false);
        exit(1);
    }

    std::ranges::sort(AllContract, std::less<>());
    Utils::GetClientList(_userId);
    Utils::CreateSupportFolder();
    ConfigLoader::Instance();

    _templateBuilderPtr   = std::make_unique<TemplateBuilder>(_showTemplateBuilder);
    _positionPtr          = std::make_unique<Position>(_executor);
    _strategyWorkspacePtr = std::make_unique<StrategyWorkspace>(_strand);
    _tradeHistoryPtr      = std::make_unique<TradeHistory>();
    _optionChainPtr       = std::make_unique<OptionChain>();
    _multicastReceiverPtr = std::make_unique<MulticastReceiver>(_executor, _marketEventQueue);
    _orderBookPtr         = std::make_unique<OrderBook>(ORDER_ALL_BOOK);
    _rejectBookPtr        = std::make_unique<OrderBook>(REJECT_BOOK);

    _marketWatchPtr = std::make_unique<MarketWatch>(_orderFormPtr, _showMarketWatch, _showPriceLadder, [&](const std::string& contract_) {
        _optionChainPtr->SetOptionForFuture(contract_);
    });

    _openOrdersPtr = std::make_unique<OpenOrders>(_orderFormPtr, _strand, _showOpenOrders, [this](const OrderInfoPtrT& info_) {
        CancelOrderEvent(info_);
    });

    _messageBroker = std::make_unique<MessageBroker>(_executor, _userId, [&](const OrderInfoPtrT& orderInfo_) {
        AddTrade(orderInfo_);
    });

    _orderFormPtr = std::make_unique<OrderForm>(_strand, [&](const OrderFormInfoT& info_, Lancelot::RequestType type_) {
        ManualOrderRequestEvent(info_, type_);
    });

    Imports(TRADING_APP_CONFIG_PATH);
    SetTheme(static_cast<VisualTheme>(_theme));
    {
        auto callback                      = [&](const StrategyRowPtrT& row_, const std::string& name_, Lancelot::RequestType type_) { StrategyRequestEvent(row_, name_, type_); };
        PortfolioInterface::StrategyAction = std::move(callback);
    }
    {
        auto callback                           = [&](const std::string& contract_) { _marketWatchPtr->AddContractToMarketWatch(contract_); };
        PortfolioInterface::AddContractFunction = std::move(callback);
    }

    StartAllThreads();
}

Arthur::~Arthur() {
    Exports(TRADING_APP_CONFIG_PATH);
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
        _executor.stop();
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

void Arthur::Paint() {
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

    Menu();
    if (_showDemoWindow) {
        ImGui::ShowDemoWindow(&_showDemoWindow);
    }

    _positionPtr->Paint(&_showPosition);
    _marketWatchPtr->Paint();
    _openOrdersPtr->Paint();
    _strategyWorkspacePtr->Paint(&_showStrategyWorkspace);
    _tradeHistoryPtr->paint(&_showTradeHistory);
    _optionChainPtr->Paint(&_showOptionChain);
    _orderBookPtr->paint(&_showOrderBook);
    _rejectBookPtr->paint(&_showRejectBook);
    Utils::StatusBar();
}

void Arthur::AddTrade(const OrderInfoPtrT& tradeInfo_) {
    switch (tradeInfo_->_statusValue) {
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
                ImGui::OpenPopup(TemplateBuilder::BeginColumnGenerator);
            }
            if (_showTemplateBuilder) {
                _templateBuilderPtr->Paint(&_showTemplateBuilder);
            }

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
            if (ImGui::MenuItem(ICON_MD_DEVELOPER_MODE " Demo")) {
                _showDemoWindow = not _showDemoWindow;
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
        ImGui::TextColored(UpDownColor(_messageBroker->IsConnected()), "%s%s:%hu", ICON_MD_LAN, _backend._address.data(), _backend._port);
        ImGui::SameLine();

        ImGui::EndMainMenuBar();
    }
}

auto Arthur::Run(std::stop_token& stopToken_) -> void {
    while (not stopToken_.stop_requested()) {
        boost::system::error_code errorCode;
        _executor.run(errorCode);
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

auto Arthur::Imports(std::string_view path_) -> void {
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

auto Arthur::Exports(std::string_view path_) -> void {
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

void Arthur::StartAllThreads() {
    {
        auto thread = std::make_unique<std::jthread>([&](std::stop_token token_) { Run(token_); });
        _threadGroup.push_back(std::move(thread));
    }

    if (false) {
        auto thread = std::make_unique<std::jthread>([&](std::stop_token token_) { MarketEventHandler(token_); });
        _threadGroup.push_back(std::move(thread));
    }

    _multicastReceiverPtr->BindMc(_marketWatch._address, _marketWatch._port);
    _multicastReceiverPtr->Read();

    _messageBroker->MakeConnection(_backend._address, _backend._port);
}

void Arthur::MarketEventHandler(std::stop_token& stopToken_) {
    while (not stopToken_.stop_requested()) {
        _marketEventQueue.consume_all([&](MarketWatchDataPtrT pointer_) { Scanner::GetInstance().Process(pointer_->_token); });
    }
    LOG(WARNING, "{} {}", __FUNCTION__, "Exiting")
}

auto MemoryUsage::GetRamUsage() -> double {
#if _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(PROCESS_MEMORY_COUNTERS_EX));
    return (double)pmc.WorkingSetSize / (1024 * 1024);
#else
    return 0.0;
#endif
}

void Arthur::ManualOrderRequestEvent(const OrderFormInfoT& info_, Lancelot::RequestType type_) {
    switch (type_) {
        case Lancelot::RequestType_LOGIN: {
            break;
        }
        case Lancelot::RequestType_NEW: {
            Lancelot::ManualOrder order{
                ._header = {
                    ._type   = type_,
                    ._length = sizeof(Lancelot::ManualOrder) - sizeof(Lancelot::Header),
                },
                ._user = {
                    ._user      = _userId,
                    ._portfolio = 9999,
                },
                ._token         = static_cast<uint32_t>(info_._marketWatch->_token),
                ._price         = static_cast<uint32_t>(info_._price),
                ._quantity      = static_cast<uint32_t>(info_._quantity),
                ._triggerPrice  = 0,
                ._side          = info_._side,
                ._orderSequence = 0,
                ._orderType     = static_cast<int16_t>(info_._type),
                ._nnf           = 0,
            };
            _messageBroker->WriteSync(&order, sizeof(Lancelot::ManualOrder));
            break;
        }
        case Lancelot::RequestType_MODIFY: {
            Lancelot::ModifyOrder order{
                ._header = {
                    ._type   = type_,
                    ._length = sizeof(Lancelot::ModifyOrder) - sizeof(Lancelot::Header),
                },
                ._user = {
                    ._user      = _userId,
                    ._portfolio = 9999,
                },
                ._token         = static_cast<uint32_t>(info_._marketWatch->_token),
                ._orderSequence = static_cast<int32_t>(info_._uniqueId),
                ._price         = static_cast<uint32_t>(info_._price),
                ._quantity      = static_cast<uint32_t>(info_._quantity),
                ._triggerPrice  = 0,
            };
            _messageBroker->WriteSync(&order, sizeof(Lancelot::ModifyOrder));
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

void Arthur::StrategyRequestEvent(StrategyRowPtrT row_, const std::string& name_, Lancelot::RequestType type_) {
    auto buffer = Utils::StrategySerialize(row_, name_, type_);

    Lancelot::StrategyHeader header{
        ._header = {
            ._type   = type_,
            ._length = static_cast<int16_t>(buffer.length() + sizeof(Lancelot::UserPortfolio)),
        },
        ._user = {
            ._user      = _userId,
            ._portfolio = static_cast<int16_t>(row_->_portfolio),
        }};
    _messageBroker->WriteSync(&header, sizeof(header));
    _messageBroker->WriteSync(buffer.data(), buffer.length());
}

void Arthur::CancelOrderEvent(const OrderInfoPtrT& orderInfo_) {
    Lancelot::CancelOrder order{
        ._header = {
            ._type   = Lancelot::RequestType_CANCEL,
            ._length = sizeof(Lancelot::CancelOrder) - sizeof(Lancelot::Header),
        },
        ._user = {
            ._user      = _userId,
            ._portfolio = 9999,
        },
        ._token         = orderInfo_->_token,
        ._orderSequence = static_cast<int16_t>(orderInfo_->_uniqueId),

    };
    _messageBroker->WriteSync(&order, sizeof(Lancelot::CancelOrder));
}
