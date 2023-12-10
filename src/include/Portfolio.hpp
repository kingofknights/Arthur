#pragma once

#include <future>

#include "PortfolioInterface.hpp"
#include "Signal.hpp"

enum ExportImport : int;

class Portfolio final : public PortfolioInterface {
  public:
    Portfolio(const std::string& workspaceName_, const std::string& strategyName_, boost::asio::io_context::strand& strand_);
    void paint();
    void AddScannerPortfolio(const ParameterInfoListT& list_);

    [[nodiscard]] std::string getStrategyName() const;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] bool        closed() const;

    static void setCallback(const AddContractToMarketWatchSignal::slot_type& slot_);

  protected:
    static AddContractToMarketWatchSignal _addContractToMarketWatchSignal;

    void DrawPortfolioWindow();
    void DrawNewPortfolioCreation();
    void DrawStrategyRow(StrategyRowPtrT& row_, int index_);
    void DrawGlobalParam();
    void DrawNewStrategyPopUpWindow();
    void DrawGlobalParamPopupWindow();
    void DrawFileManagerWindow();
    void AppendStrategy();
    void ModifyGlobalParam();
    void ResetSelection();
    void RemoveSelection();

  private:
    bool _showScanner            = false;
    bool _showGlobalParameter    = false;
    int  _toBeDeleted            = -1;
    int  _multipleSelectionCount = 0;

    std::string      _exchange;
    ExportImport     _action;
    PortfolioStatusT _status{
        .Close     = false,
        .Inactive  = 0,
        .Active    = 0,
        .Apply     = 0,
        .Waiting   = 0,
        .Terminate = 0,
    };
    ImGuiListClipper  _clipper;
    ImGuiListClipper  _contractClipper;
    std::future<void> _future;
    ScannerAddQueueT  _scannerAddQueue;
};
