//
// Created by VIKLOD on 22-01-2023.
//

#pragma once

#include "Signal.hpp"
#include "Structure.hpp"

class OrderForm;
using OrderFormPtrT = std::shared_ptr<OrderForm>;

class MarketWatch final {
  public:
    explicit MarketWatch(const OrderFormPtrT& manualOrder_);

    ~MarketWatch();

    static void ToolTipDisplay(const MarketWatchDataPtrT& pointer_);

    static void LadderView(const MarketWatchDataPtrT& pointer_);

    void Paint(bool* showMarketWatch_, bool* showLadder_);

    void AddContractToMarketWatch(const std::string& contract_);

    void Connect(OptionChainContractSlotT callback_);

  protected:
    void DrawMarketWatchTable(bool* show_);

    void DrawLadderWatchWindow(bool* show_);

    void ContractCell(int contract_, int index_, const char* data_, const MarketWatchDataPtrT& pointer_);

    void Imports(const std::string& path_);

    void Exports(const std::string& path_);

    void DrawColumn(const MarketWatchDataPtrT& data_, int index_);

    void Remove();

    void DrawSearchBox();

  private:
    const OrderFormPtrT        _manualOrderPtr;
    MarketWatchDataPtrT        _ladderDataPtr;
    OptionChainContractSignalT _optionChainContractSignal;

    int  _toBeDeleted  = -1;
    int  _selectedRow  = -1;
    bool _searchOrDrop = false;

    std::string      _currentContract;
    std::string      _month;
    LiveContainerT   _liveUpdates;
    SubscribedT      _subscribed;
    ImGuiListClipper _clipper;
    ImGuiTextFilter  _filter;
};
