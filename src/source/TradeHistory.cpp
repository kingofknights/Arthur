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

            ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            for (int column = 0; column < BooksColumnIndex_END; ++column) {
                ImGui::TableSetColumnIndex(column);
                ImGui::PushID(column);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                if (ImGui::SmallButton(ICON_MD_ARROW_DROP_DOWN)) {
                    _currentFilterOpended = column;
                    _filterWindowShow     = true;
                    FillFilterOption();
                    ImGui::OpenPopup(FORMAT("{} Filter", BookTableColumnName[column]).data());
                }
                ImGui::PopStyleVar();
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::TableHeader(BookTableColumnName[column]);
                ImGui::PopID();
            }
            if (not _filterWindowShow) {
                _currentFilterOpended = -1;
            } else {
                FilterOptionsWindows();
            }
            // ImGui::TableHeadersRow();
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

void TradeHistory::FilterOptionsWindows() {
    int index = _currentFilterOpended;
    if (index == -1 or index >= BooksColumnIndex_END) {
        return;
    }
    ImGui::PushID(index);
    if (ImGui::BeginPopupContextItem(FORMAT("{} Filter", BookTableColumnName[index]).data(), ImGuiPopupFlags_None)) {
        ImGui::Columns(2, nullptr, false);
        if (ImGui::Button(FORMAT("{} Clear", ICON_MD_DELETE).data(), ImVec2{-FLT_MIN, 0})) {
            std::ranges::for_each(_filter[index], [](auto& item_) {
                item_.second = false;
            });
            StartNewFilter();
        }

        ImGui::NextColumn();
        if (ImGui::Button(FORMAT("{} Clear All", ICON_MD_DELETE_SWEEP).data(), ImVec2{-FLT_MIN, 0})) {
            std::ranges::for_each(_filter, [](auto& item_) {
                item_.clear();
            });
            _isFilterActive = IsFilterActive();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndColumns();
        auto& container = _filter[index];
        if (ImGui::BeginListBox(FORMAT("##{}", BookTableColumnName[index]).data())) {
            for (auto& item : container) {
                if (ImGui::Checkbox(item.first.data(), &item.second)) {
                    StartNewFilter();
                }
            }
            ImGui::EndListBox();
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void TradeHistory::StartNewFilter() {
    _isFilterActive = IsFilterActive();
    _filterContainer.clear();
    if (not _isFilterActive) {
        return;
    }

    for (const auto& order : _container) {
        if (Utils::CheckPassFiler(order, _filter, _active)) {
            _filterContainer.push_back(order);
        }
    }
}

auto TradeHistory::IsFilterActive() -> bool {
    std::ranges::transform(_filter, _active.begin(), [](const auto& container_) {
        return std::ranges::any_of(container_, [](const auto& pair_) {
            return pair_.second;
        });
    });
    return std::ranges::any_of(_active, [](bool value_) {
        return value_;
    });
}
void TradeHistory::FillFilterOption() {
    if (not _isFilterActive) {
        std::ranges::for_each(_filter, [](auto& item_) { item_.clear(); });
    } else {
        std::ranges::for_each(_filter, [](auto& item_) {
            std::erase_if(item_, [](const auto& pair_) { return !pair_.second; });
        });
    }
    const auto& local = _isFilterActive ? _filterContainer : _container;
    std::ranges::for_each(local, [&](const auto& pair_) {
        const OrderInfoPtrT& order = pair_;
        _filter[BooksColumnIndex_PF].emplace(FORMAT("{}", order->_portfolio), false);
        _filter[BooksColumnIndex_CONTRACT].emplace(FORMAT("{}", order->_contract), false);
        _filter[BooksColumnIndex_PRICE].emplace(FORMAT("{:.2f}", order->_price), false);
        _filter[BooksColumnIndex_QUANTITY].emplace(FORMAT("{}", order->_quantity), false);
        _filter[BooksColumnIndex_FILL_PRICE].emplace(FORMAT("{:.2f}", order->_fillPrice), false);
        _filter[BooksColumnIndex_FILL_QUANTITY].emplace(FORMAT("{}", order->_fillQuantity), false);
        _filter[BooksColumnIndex_REMAINING_QTY].emplace(FORMAT("{}", order->_remaining), false);
        _filter[BooksColumnIndex_CLIENT].emplace(FORMAT("{}", order->_client), false);
        _filter[BooksColumnIndex_STATUS].emplace(FORMAT("{}", OrderStatusInfoName[order->_statusValue]), false);
        _filter[BooksColumnIndex_TIME].emplace(FORMAT("{}", order->_time), false);
        _filter[BooksColumnIndex_GATEWAY].emplace(FORMAT("{}", order->_uniqueId), false);
        _filter[BooksColumnIndex_ORDER_NUMBER].emplace(FORMAT("{}", order->_orderNumber), false);
        _filter[BooksColumnIndex_MESSAGE].emplace(FORMAT("{}", order->_message), false);
    });
}
