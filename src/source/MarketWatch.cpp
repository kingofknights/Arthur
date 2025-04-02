//
// Created by VIKLOD on 22-01-2023.
//

#include "../include/MarketWatch.hpp"

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Colors.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/OrderForm.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Utils.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <memory>
#include <utility>

extern AllContractT AllContract;

#define MARKET_WATCH_CONFIG_PATH "Config/MarketWatch.json"

MarketWatch::MarketWatch(const OrderFormPtrT& manualOrder_, bool& showMarketWatch_, bool& showLadder_, AddContractFunctionT function_)
    : _manualOrder{manualOrder_},
      _showMarketWatch{showMarketWatch_},
      _showMarketLadder{showLadder_},
      _function{std::move(function_)},
      _ladderDataPtr{std::make_shared<MarketWatchDataT>()} {
    Imports(MARKET_WATCH_CONFIG_PATH);
}

MarketWatch::~MarketWatch() {
    Exports(MARKET_WATCH_CONFIG_PATH);
}

void MarketWatch::DrawMarketWatchTable() noexcept {
    if (_toBeDeleted != -1) {
        Remove();
    }

    if (ImGui::Begin("MarketWatch", &_showMarketWatch)) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Stock");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 4);
        if (_searchOrDrop) {
            _filter.Draw("##searchbar");
            DrawSearchBox();
        } else {
            if (ImGui::BeginCombo("##AllContracts", _currentContract.data())) {
                _clipper.Begin(static_cast<int>(AllContract.size()));
                while (_clipper.Step()) {
                    auto begin = AllContract.begin() + _clipper.DisplayStart;
                    auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
                    for (auto iterator = begin; iterator < end; ++iterator) {
                        if (ImGui::Selectable(iterator->data())) {
                            _currentContract = *iterator;
                        }
                    }
                }
                ImGui::EndCombo();
            }
        }
        ImGui::SameLine();
        ImGui::Checkbox("Filter", &_searchOrDrop);
        ImGui::SameLine();
        ImGui::BeginDisabled(_currentContract.empty());
        if (ImGui::Button(ICON_MD_ADD_BOX " Add Stock")) {
            AddContractToMarketWatch(_currentContract);
            _filter.Clear();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (_searchOrDrop) {
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 8);
            ImGui::InputText("Filter##_month", &_month, ImGuiInputTextFlags_CharsUppercase);
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_MD_ALL_INCLUSIVE " Future")) {
            for (const auto& contract : AllContract) {
                if (contract.starts_with("FUT") and (_month.empty() ? true : (contract.find(_month) != std::string::npos))) {
                    AddContractToMarketWatch(contract);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_MD_CLEAR_ALL " Clear All")) {
            _liveUpdates.clear();
            _subscribed.clear();
        }
        ImGui::SameLine();
        ImGui::Text("Subscribed: %zu", _subscribed.size());

        if (ImGui::BeginTable("Market Watch Table", MarketWatchColumnIndex_END, TableFlags)) {
            ImGui::TableSetupScrollFreeze(1, 0);
            for (const auto& columnName : MarketWatchTableColumnName) {
                ImGui::TableSetupColumn(columnName, TableColumnFlags);
            }
            ImGui::TableHeadersRow();

            _clipper.Begin(static_cast<int>(_liveUpdates.size()));
            while (_clipper.Step()) {
                auto begin = _liveUpdates.begin() + _clipper.DisplayStart;
                auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
                int  index = _clipper.DisplayStart;
                for (auto iterator = begin; iterator < end; ++iterator) {
                    ImGui::TableNextRow();
                    DrawColumn(*iterator, index);
                    ++index;
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void MarketWatch::ToolTipDisplay(const MarketWatchDataPtrT& pointer_) noexcept {
    if (ImGui::BeginTooltip()) {
        MarketWatch::LadderView(pointer_);
        ImGui::EndTooltip();
    }
}

void MarketWatch::ContractCell(int contract_, int index_, const char* data_, const MarketWatchDataPtrT& pointer_) {
    FirstCell(index_, data_, _selectedRow, contract_);
    if (_selectedRow == contract_ and ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
        if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
            _toBeDeleted = contract_;
        }

        if (ImGui::BeginPopupContextItem("Context Menu", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::Selectable(ICON_MD_ADD_BOX " Show option chain")) {
                _function(data_);
            }

            ImGui::EndPopup();
        }

        bool           open = false;
        Lancelot::Side side;
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadAdd) or ImGui::IsKeyPressed(ImGuiKey_F1)) {
            open = true;
            side = Lancelot::Side_BUY;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract) or ImGui::IsKeyPressed(ImGuiKey_F2)) {
            open = true;
            side = Lancelot::Side_SELL;
        }

        if (open) {
            OrderFormInfoT info{
                ._uniqueId    = 0,
                ._price       = pointer_->_lastTradePrice,
                ._quantity    = (int)Lancelot::ContractInfo::GetLotSize(pointer_->_token),
                ._lotSize     = info._quantity,
                ._orderNumber = 0,
                ._type        = 0,
                ._side        = side,
                ._status      = OrderStatus_NEW,
                ._contract    = Lancelot::ContractInfo::GetDescription(pointer_->_token),
                ._client      = "Pro",
                ._marketWatch = pointer_,
            };
            _manualOrder->Update(info);
            ImGui::OpenPopup(NEW_ORDER_WINDOW);
        }
        _manualOrder->Paint(NEW_ORDER_WINDOW);
    }
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            _selectedRow   = contract_;
            _ladderDataPtr = pointer_;
        }
        MarketWatch::ToolTipDisplay(pointer_);
    }
}

void MarketWatch::LadderView(const MarketWatchDataPtrT& pointer_) noexcept {
    ImGui::LabelText("Contract", "%s", pointer_->_description.data());
    ImGui::Separator();
    if (ImGui::BeginTable("Market Watch Table ToolTip", MarketWatchToolTipColumnIndex_END)) {
        for (const auto& columnName : MarketWatchTableToolTipColumnName) {
            ImGui::TableSetupColumn(columnName, ImGuiTableColumnFlags_WidthStretch);
        }
        ImGui::TableHeadersRow();
        for (size_t i = 0; i < MarketWatchLadderCount; ++i) {
            ImGui::TableNextRow();
            NextCell(MarketWatchToolTipColumnIndex_BUY_ORDER, pointer_->_bid[i]._order, BuySellColor(Lancelot::Side_BUY));
            NextCell(MarketWatchToolTipColumnIndex_BUY_QUANTITY, pointer_->_bid[i]._quantity, BuySellColor(Lancelot::Side_BUY));
            NextCell(MarketWatchToolTipColumnIndex_BUY_PRICE, pointer_->_bid[i]._price, BuySellColor(Lancelot::Side_BUY));
            NextCell(MarketWatchToolTipColumnIndex_ASK_PRICE, pointer_->_ask[i]._price, BuySellColor(Lancelot::Side_SELL));
            NextCell(MarketWatchToolTipColumnIndex_ASK_QUANTITY, pointer_->_ask[i]._quantity, BuySellColor(Lancelot::Side_SELL));
            NextCell(MarketWatchToolTipColumnIndex_ASK_ORDER, pointer_->_ask[i]._order, BuySellColor(Lancelot::Side_SELL));
        }
        ImGui::EndTable();
    }
    ImGui::Columns(2, nullptr, false);

    ImGui::LabelText("Open", "%.2f", pointer_->_open);
    ImGui::LabelText("Low", "%.2f", pointer_->_low);
    ImGui::LabelText("LTP", "%.2f", pointer_->_lastTradePrice);

    ImGui::NextColumn();
    ImGui::LabelText("High", "%.2f", pointer_->_high);
    ImGui::LabelText("Close", "%.2f", pointer_->_close);
    ImGui::LabelText("ATP", "%.2f", pointer_->_averageTradePrice);
    ImGui::EndColumns();

    auto  range  = (pointer_->_high - pointer_->_low);
    float moment = (pointer_->_lastTradePrice - pointer_->_low) / std::max(range, 1.0F);
    LOG(INFO, "{}", range);
    uint64_t total = (pointer_->_totalBuyQuantity + pointer_->_totalSellQuantity);
    float    ratio = float(pointer_->_totalBuyQuantity) / float(total == 0 ? 1 : total);

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, BuySellColor(Lancelot::Side_BUY));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, BuySellColor(Lancelot::Side_SELL));

    ImGui::ProgressBar(moment, ImVec2(-FLT_MIN, 0), "Price Movement");
    ImGui::ProgressBar(ratio, ImVec2(-FLT_MIN, 0), "Buy Sell Ratio");

    ImGui::PopStyleColor(2);
}

void MarketWatch::AddContractToMarketWatch(const std::string& contract_) {
    auto token = Lancelot::ContractInfo::GetToken(contract_);
    if (not _subscribed.contains(token)) {
        auto ref = ContractInfo::GetLiveDataRef(token);
        if (not ref) {
            return;
        }
        _liveUpdates.push_back(ref);
        _subscribed.emplace(token);
    }
}

void MarketWatch::Paint() {
    if (_showMarketWatch) {
        DrawMarketWatchTable();
    }
    if (_showMarketLadder) {
        DrawLadderWatchWindow();
    }
}

void MarketWatch::DrawLadderWatchWindow() noexcept {
    if (ImGui::Begin("MarketWatchLadder", &_showMarketWatch)) {
        if (_ladderDataPtr) {
            MarketWatch::LadderView(_ladderDataPtr);
        }
    }
    ImGui::End();
}

void MarketWatch::Imports(const std::string& path_) {
    std::fstream file(path_, std::ios::in);
    if (not file.is_open()) {
        return;
    }

    nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);
    std::ranges::for_each(root.items(), [&](auto& value_) { AddContractToMarketWatch(value_.key()); });

    file.close();
}

void MarketWatch::Exports(const std::string& path_) {
    if (_subscribed.empty()) {
        std::remove(path_.data());
        return;
    }
    nlohmann::ordered_json root;

    for (const LiveContainerT::value_type& valueType : _liveUpdates) {
        root[valueType->_description.data()] = valueType->_token;
    }

    std::fstream file(path_, std::ios::out | std::ios::trunc);
    if (not file.is_open()) {
        return;
    }
    file << root.dump();
    file.close();
}

void MarketWatch::DrawColumn(const MarketWatchDataPtrT& data_, int index_) {
    ContractCell(index_, MarketWatchColumnIndex_CONTACT_NAME, data_->_description.data(), data_);
    NextCell(MarketWatchColumnIndex_ATP, data_->_averageTradePrice, UpDownColor(data_->_color._atp));
    NextCell(MarketWatchColumnIndex_LTP, data_->_lastTradePrice, UpDownColor(data_->_color._ltp));
    NextCell(MarketWatchColumnIndex_LTQ, data_->_lastTradePrice);
    NextCell(MarketWatchColumnIndex_LTT, data_->_lastTradeTime.data());
    NextCell(MarketWatchColumnIndex_TOP_BID, data_->_bid[0]._price, UpDownColor(data_->_color._topBid));
    NextCell(MarketWatchColumnIndex_TOP_ASK, data_->_ask[0]._price, UpDownColor(data_->_color._topAsk));
    NextCell(MarketWatchColumnIndex_OPEN, data_->_open);
    NextCell(MarketWatchColumnIndex_HIGH, data_->_high);
    NextCell(MarketWatchColumnIndex_LOW, data_->_low);
    NextCell(MarketWatchColumnIndex_CLOSE, data_->_close);
    NextCell(MarketWatchColumnIndex_LOWDPR, data_->_lowDpr);
    NextCell(MarketWatchColumnIndex_HIGHDPR, data_->_highDpr);
    NextCell(MarketWatchColumnIndex_TOTAL_BUY_QUANTITY, data_->_totalBuyQuantity);
    NextCell(MarketWatchColumnIndex_TOTAL_SELL_QUANTITY, data_->_totalSellQuantity);
    NextCell(MarketWatchColumnIndex_VOLUME_TRADED_TODAY, data_->_volumeTradedToday);
    NextCell(MarketWatchColumnIndex_OPEN_INTEREST, data_->_openInterest);
}

void MarketWatch::Remove() {
    _subscribed.erase(_ladderDataPtr->_token);
    auto iterator = _liveUpdates.erase(_liveUpdates.begin() + _toBeDeleted);

    _toBeDeleted = -1;

    if (iterator != _liveUpdates.end()) {
        _selectedRow   = (int)std::distance(_liveUpdates.begin(), iterator);
        _ladderDataPtr = *iterator;
    } else if (not _liveUpdates.empty()) {
        --iterator;
        _selectedRow   = (int)std::distance(_liveUpdates.begin(), iterator);
        _ladderDataPtr = *iterator;
    } else {
        _ladderDataPtr.reset();
    }
}

void MarketWatch::DrawSearchBox() {
    if (_filter.IsActive()) {
        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        if (ImGui::Begin("Contract Filter", nullptr, OverlayFlags)) {
            Utils::ContractFilter(_filter, _currentContract);
        }
        ImGui::End();
    }
}
