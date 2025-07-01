#include "TradeHistory.hpp"

#include "API/Common.hpp"
#include "Colors.hpp"
#include "Configuration.hpp"
#include "Enums.hpp"
#include "OrderHistory.hpp"
#include "TableColumnInfo.hpp"
#include "Utils.hpp"
#include "imgui.h"
#include "imgui_internal.h"

void TradeHistory::paint(bool* show_) {
    _pendingTradeUpdate.consume_all([this](const OrderInfoPtrT& orderInfo_) {
        _container.push_back(orderInfo_);
        _totalBuy += orderInfo_->_side == Lancelot::Side_BUY;
        _totalSell += orderInfo_->_side == Lancelot::Side_SELL;
        _buyValue += orderInfo_->_side == Lancelot::Side_BUY ? orderInfo_->_price * orderInfo_->_quantity : 0;
        _sellValue += orderInfo_->_side == Lancelot::Side_SELL ? orderInfo_->_price * orderInfo_->_quantity : 0;
        _netValue = _totalSell - _totalBuy;
    });
    if (*show_) {
        DrawTradeBookTable(show_);
    }
}

void TradeHistory::Insert(const OrderInfoPtrT& orderInfo_) {
    _pendingTradeUpdate.push(orderInfo_);
}

void TradeHistory::DrawTradeBookTable(bool* show_) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5F, 0.5F));
    ImGui::SetNextWindowSize(ImVec2{ImGui::GetMainViewport()->Size.x / 2, ImGui::GetMainViewport()->Size.y / 2}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Trade Book", show_)) {
        const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginTable("Trade Book Table", BooksColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
            ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible
            for (const auto& name : BookTableColumnName) {
                ImGui::TableSetupColumn(name, TableColumnFlags);
            }

            auto* table                      = ImGui::GetCurrentTable();
            table->DisableDefaultContextMenu = true;
            if (ImGui::TableBeginContextMenuPopup(table)) {
                if (ImGui::Button(ICON_MD_FILTER " Filter")) {
                    ImGui::OpenPopup("Open Orders Filter");
                }
                FilterOptionsWindows();
                if (ImGui::BeginMenu(ICON_MD_MORE " More ...")) {
                    ImGui::TableDrawDefaultContextMenu(table, TableFlags);
                    ImGui::EndMenu();
                }

                ImGui::EndPopup();
            }

            ImGui::TableHeadersRow();
            const auto& container = _isFilterActive ? _filterContainer : _container;
            _clipper.Begin(container.size());
            while (_clipper.Step()) {
                auto begin = container.rbegin() + _clipper.DisplayStart;
                auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
                int  i     = _clipper.DisplayStart;
                for (auto iterator = begin; iterator < end; ++iterator, ++i) {
                    ImGui::TableNextRow();
                    const OrderInfoPtrT& tradeInfo_ = *iterator;
                    ImGui::PushID(i);
                    Utils::DrawTradeRow(tradeInfo_, _selectedRow, i);
                    if (_selectedRow == i) {
                        if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
                            OrderHistory::Instance().LoadOrderHistory(tradeInfo_->_orderNumber);
                            ImGui::OpenPopup(ORDER_HISTORY_POPUP_WINDOW);
                        }
                        OrderHistory::Instance().Paint(nullptr);
                    }
                    ImGui::PopID();
                }
            }

            ImGui::EndTable();
        }
        ImGui::Separator();
        ImGui::Text("| Total : [%zu] |", _container.size());
        ImGui::SameLine();
        ImGui::TextColored(UpDownColor(true), "| Buy : [%d] |", _totalBuy);
        ImGui::SameLine();
        ImGui::TextColored(UpDownColor(true), "| Buy Value : [%.2f] |", _buyValue);
        ImGui::SameLine();
        ImGui::TextColored(UpDownColor(false), "| Sell : [%d] |", _totalSell);
        ImGui::SameLine();
        ImGui::TextColored(UpDownColor(false), "| Sell Value : [%.2f] |", _sellValue);
        ImGui::SameLine();
        ImGui::TextColored(UpDownColor(_netValue > 0), "| Net Value : [%.2f] |", _netValue);
        if (_isFilterActive) {
            ImGui::SameLine();
            ImGui::Text("%s", "Filter is active");
        }
    }
    ImGui::End();
}
void TradeHistory::PFFilter() {
    if (ImGui::Button(ICON_MD_CLEAR_ALL " Clear", ImVec2{-FLT_MIN, 0})) {
        _pfFilter.clear();
        StartNewFilter();
    }
    for (const auto& item : _container) {
        _pfFilter.emplace(item->_portfolio, false);
    }
    if (ImGui::BeginListBox("##PFOptions")) {
        for (auto& item : _pfFilter) {
            if (ImGui::Checkbox(FORMAT("{}", item.first).data(), &item.second)) {
                StartNewFilter();
            }
        }
        ImGui::EndListBox();
    }
}
void TradeHistory::FilterOptionsWindows() {
    if (ImGui::BeginPopupContextWindow("Open Orders Filter")) {
        if (ImGui::BeginTabBar("Filter")) {
            if (ImGui::BeginTabItem("Portfolio")) {
                PFFilter();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Contract")) {
                ContractFilter();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndPopup();
    }
}
void TradeHistory::ContractFilter() {
    if (ImGui::Button(ICON_MD_CLEAR_ALL " Clear", ImVec2{-FLT_MIN, 0})) {
        _symbolFilter.clear();
        StartNewFilter();
    }
    for (const auto& item : _container) {
        _symbolFilter.emplace(item->_contract, false);
    }
    if (ImGui::BeginListBox("##ContractOptions")) {
        for (auto& item : _symbolFilter) {
            if (ImGui::Checkbox(FORMAT("{}", item.first).data(), &item.second)) {
                StartNewFilter();
            }
        }
        ImGui::EndListBox();
    }
}
void TradeHistory::StartNewFilter() {
    _filterContainer.clear();
    auto found1     = std::ranges::any_of(_pfFilter, [](const auto& pair_) {
        return pair_.second;
    });
    auto found2     = std::ranges::any_of(_symbolFilter, [](const auto& pair_) {
        return pair_.second;
    });
    _isFilterActive = found1 or found2;

    LOG(INFO, "_pfFilter {}, _symbolFilter {} active {}", found1, found2, _isFilterActive)
    if (_isFilterActive) {
        std::ranges::for_each(_container, [this](const auto& trade_) {
            DoFilter(trade_);
        });
    }
}
void TradeHistory::DoFilter(const OrderInfoPtrT& tradeInfo_) {
    if (_pfFilter.empty() and _symbolFilter.empty()) {
        return;
    }
    if (not _pfFilter.empty() and std::ranges::any_of(_pfFilter, [](const auto& item_) { return item_.second; })) {
        const auto iterator = _pfFilter.find(tradeInfo_->_portfolio);
        if (iterator != _pfFilter.cend()) {
            if (not iterator->second) {
                return;
            }
        } else {
            return;
        }
    }
    if (not _symbolFilter.empty() and std::ranges::any_of(_symbolFilter, [](const auto& item_) { return item_.second; })) {
        const auto iterator = _symbolFilter.find(tradeInfo_->_contract);
        if (iterator != _symbolFilter.cend()) {
            if (not iterator->second) {
                return;
            }
        } else {
            return;
        }
    }
    _filterContainer.push_back(tradeInfo_);
}
