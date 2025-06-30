#pragma once

#include "Arthur_Fwd.hpp"
#include "PortfolioInterface.hpp"

#include <future>

enum ExportImport : int;

class Portfolio final : public PortfolioInterface {
  public:
    Portfolio(const std::string& workspaceName_, const std::string& strategyName_, ExecutorT& strand_);

    void Paint();

    void AddScannerPortfolio(const ParameterInfoListT& list_);

    [[nodiscard]] auto Closed() const -> bool;

  protected:
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
    int  _toBeDeleted            = -1;
    int  _multipleSelectionCount = 0;

    std::string      _exchange;
    ExportImport     _action;
    PortfolioStatusT _status{
        ._close     = false,
        ._inactive  = 0,
        ._active    = 0,
        ._apply     = 0,
        ._waiting   = 0,
        ._terminate = 0,
    };
    ImGuiListClipper  _clipper;
    ImGuiListClipper  _contractClipper;
    std::future<void> _future;
    ScannerAddQueueT  _scannerAddQueue;
};
