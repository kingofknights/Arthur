//
// Created by VIKLOD on 22-01-2023.
//

#pragma once

#include "Arthur_Fwd.hpp"

#include <deque>
#include <set>

class MarketWatch final {
    using LiveContainerT = std::deque<MarketWatchDataPtrT>;
    using SubscribedT    = std::set<uint32_t>;

  public:
    MarketWatch(const OrderFormPtrT& manualOrder_, const TokenFilterPtrT& tokenFilter_, bool& showMarketWatch_, bool& showLadder_, AddContractFunctionT function_);

    ~MarketWatch();

    static void ToolTipDisplay(const MarketWatchDataPtrT& pointer_) noexcept;

    static void LadderView(const MarketWatchDataPtrT& pointer_) noexcept;

    void Paint();

    void AddContractToMarketWatch(const std::string& contract_);

  protected:
    void DrawMarketWatchTable() noexcept;

    void DrawLadderWatchWindow() noexcept;

    void ContractCell(int contract_, int index_, const char* data_, const MarketWatchDataPtrT& pointer_);

    void Imports(const std::string& path_);

    void Exports(const std::string& path_);

    void DrawColumn(const MarketWatchDataPtrT& data_, int index_);

    void Remove();

    void DrawSearchBox();

  private:
    const OrderFormPtrT&   _manualOrder;
    const TokenFilterPtrT& _tokenFilter;

    bool& _showMarketWatch;
    bool& _showMarketLadder;

    AddContractFunctionT _function;
    MarketWatchDataPtrT  _ladderDataPtr;

    int  _toBeDeleted  = -1;
    int  _selectedRow  = -1;
    bool _searchOrDrop = false;

    std::string      _currentContract;
    LiveContainerT   _liveUpdates;
    SubscribedT      _subscribed;
    ImGuiListClipper _clipper;
    ImGuiTextFilter  _filter;
};
