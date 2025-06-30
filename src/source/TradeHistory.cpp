#include "TradeHistory.hpp"

#include "API/Common.hpp"
#include "Colors.hpp"
#include "Configuration.hpp"
#include "Enums.hpp"
#include "OrderHistory.hpp"
#include "TableColumnInfo.hpp"
#include "Utils.hpp"
#include "imgui.h"

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
            ImGui::TableHeadersRow();
            const auto& container = _filter.IsActive() ? _filterContainer : _container;
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
        ImGui::SameLine();
        if (_filter.Draw()) {
            _filterContainer.clear();
            for (const auto& item : _container) {
                if (_filter.PassFilter(item->_contract.data())) {
                    _filterContainer.push_back(item);
                }
            }
        }
    }
    ImGui::End();
}
