#pragma once

#include "Arthur_Fwd.hpp"
#include "Structure.hpp"

#include <cstdint>
#include <string_view>

namespace Lancelot {
    enum RequestType : uint8_t;
}
enum VisualTheme : int;

class Arthur {
  public:
    explicit Arthur(bool* closeMainWindow_, UserDetails details_);

    ~Arthur();
    void Paint();

    void AddTrade(const OrderInfoPtrT& tradeInfo_);

  protected:
    void MarketEventHandler(std::stop_token& token_);

    void SetTheme(VisualTheme theme_);

    void Menu();

    void Run(std::stop_token& stopToken_);

    void Imports(std::string_view path_);

    void Exports(std::string_view path_);

    void StartAllThreads();

    void ManualOrderRequestEvent(const OrderFormInfoT& info_, Lancelot::RequestType type_);

    void StrategyRequestEvent(StrategyRowPtrT row_, const std::string& name_, Lancelot::RequestType type_);

    void CancelOrderEvent(const OrderInfoPtrT& orderInfo_);

    void ShowIndex(SpotInfoT& info_, const std::string& name_);

  private:
    MessageBrokerPtrT     _messageBroker;
    TemplateBuilderPtrT   _templateBuilderPtr;
    PositionPtrT          _positionPtr;
    OrderFormPtrT         _orderFormPtr;
    MarketWatchPtrT       _marketWatchPtr;
    MulticastReceiverPtrT _multicastReceiverPtr;
    OpenOrdersPtrT        _openOrdersPtr;
    StrategyWorkspacePtrT _strategyWorkspacePtr;
    TradeHistoryPtrT      _tradeHistoryPtr;
    OptionChainPtrT       _optionChainPtr;
    OrderBookPtrT         _orderBookPtr;
    OrderBookPtrT         _rejectBookPtr;
    TokenFilterPtrT       _tokenFilterPtr;

    bool _showTemplateBuilder   = false;
    bool _showDemoWindow        = false;
    bool _showExcelWindow       = false;
    bool _showPriceLadder       = false;
    bool _showMarketWatch       = false;
    bool _showOptionChain       = false;
    bool _showOpenOrders        = false;
    bool _showStrategyWorkspace = false;
    bool _showTradeHistory      = false;
    bool _showPosition          = false;
    bool _showOrderBook         = false;
    bool _showRejectBook        = false;
    int  _theme                 = 0;

    ExecutorT    _executor;
    WorkerT      _backendWorker;
    ThreadGroupT _threadGroup;
    bool*        _closeMainWindow;

    struct Connection {
        std::string _interface;
        std::string _address;
        uint16_t    _port;
    };

    UserDetails _userId;

    Connection        _backend;
    Connection        _marketWatch;
    MarketEventQueueT _marketEventQueue;
};
