//
// Created by VIKLOD on 22-01-2023.
//

#pragma once

#include "Signal.hpp"
#include "Structure.hpp"

class OrderForm;
using OrderFormPtrT = std::shared_ptr<OrderForm>;

class MarketWatch {
  public:
    MarketWatch(const OrderFormPtrT& manualOrder_);
    ~MarketWatch();

    static void ToolTipDisplay(const MarketWatchDataPtrT& pointer_);
    static void LadderView(const MarketWatchDataPtrT& pointer_);

    void paint(bool* showMarketWatch_, bool* showLadder_);
    void addContractToMarketWatch(const std::string& contract_);
    void Connect(OptionChainContractSlot callback_);

  protected:
    void DrawMarketWatchTable(bool* show_);
    void DrawLadderWatchWindow(bool* show_);

  private:
    void ContractCell(int contract_, int index_, const char* data_, const MarketWatchDataPtrT& pointer_);
    void Imports(std::string_view path_);
    void Exports(std::string_view path_);
    void DrawColumn(const MarketWatchDataPtrT& marketWatchDataPtr_, int index_);
    void Remove();
    void DrawSearchBox();

  private:
    OrderFormPtrT             _manualOrderPtr;
    MarketWatchDataPtrT       _ladderDataPtr;
    OptionChainContractSignal _optionChainContractSignal;

    int  _toBeDeleted  = -1;
    int  _selectedRow  = -1;
    bool _searchOrDrop = false;

    std::string      _currentContract;
    LiveContainerT   _liveUpdates;
    SubscribedT      _subscribed;
    ImGuiListClipper _clipper;
    ImGuiTextFilter  _filter;
};
