#include "OpenOrders.hpp"

#include "API/Common.hpp"
#include "API/ContractInfo.hpp"
#include "Colors.hpp"
#include "Configuration.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "OrderForm.hpp"
#include "OrderHistory.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"
#include "Utils.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#include <cfloat>
#include <iterator>

static constexpr char CancelAllOrderWindow[] = "Cancel All Order Window";
static constexpr char BeginOpenOrders[]      = "Open Orders";
static constexpr char BeginOpenOrdersTable[] = "Open Orders Table";
static constexpr char BeginCancelBookTable[] = "Cancel Book Table";

OpenOrders::OpenOrders(const OrderFormPtrT& manualOrder_, ExecutorStrandT& strand_, bool& show_, FunctionT function_)
    : _manualOrder{manualOrder_},
      _function{std::move(function_)},
      _strand{strand_},
      _show{show_} {}

void OpenOrders::Paint() noexcept {
    _pendingOrderUpdate.consume_all([this](const auto& pair_) { Update(pair_.first, pair_.second); });
    if (_show) {
        DrawPendingBook(&_show);
    }
}

void OpenOrders::ContextMenu(int index_) noexcept {
    ImGui::PushID(index_);
    if (ImGui::BeginPopup(FORMAT("ContextMenu_{}", index_).data())) {
        if (ImGui::BeginCombo("Options", "")) {
            ImGui::EndCombo();
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void OpenOrders::DrawPendingBook(bool* show_) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5F, 0.5F));
    ImGui::SetNextWindowSize(ImVec2{ImGui::GetMainViewport()->Size.x / 2, ImGui::GetMainViewport()->Size.y / 2}, ImGuiCond_FirstUseEver);
    if (ImGui::Begin(BeginOpenOrders, show_)) {
        const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

        if (ImGui::BeginTable(BeginOpenOrdersTable, BooksColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
            ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible
            for (const auto& name : BookTableColumnName) {
                ImGui::TableSetupColumn(name, TableColumnFlags);
            }

            if (ImGui::TableBeginContextMenuPopup(ImGui::GetCurrentTable())) {
                if (ImGui::BeginMenu("Filter")) {
                    if (ImGui::BeginMenu("PF")) {
                        if (ImGui::SmallButton(ICON_MD_CLEAR_ALL " Clear")) {
                            _pfFilter.clear();
                        }
                        if (ImGui::BeginPopupModal("PF Filter")) {
                            for (const auto& item : _container) {
                                _pfFilter.emplace(item.second->_portfolio, false);
                            }
                            if (ImGui::BeginListBox("##PFOptions")) {
                                for (auto& item : _pfFilter) {
                                    ImGui::Checkbox(FORMAT("{}", item.first).data(), &item.second);
                                }
                                ImGui::EndListBox();
                            }
                            ImGui::EndPopup();
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Contract")) {
                        if (ImGui::SmallButton(ICON_MD_CLEAR_ALL " Clear")) {
                            _symbolFilter.clear();
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("More ...")) {
                    ImGui::TableDrawDefaultContextMenu(ImGui::GetCurrentTable(), TableFlags);
                    ImGui::EndMenu();
                }

                ImGui::EndPopup();
            }
            // ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
            // int column = 0;
            // for (const auto& name : BookTableColumnName) {
            //     ImGui::TableSetColumnIndex(column);
            //     ImGui::PushID(column);
            //     ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            //     if (ImGui::SmallButton(ICON_MD_ARROW_DROP_DOWN)) {
            //         ImGui::OpenPopup(FORMAT("ContextMenu_{}", column).data());
            //     }
            //     ImGui::PopStyleVar();
            //     ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            //     ImGui::TableHeader(name);
            //     ImGui::PopID();
            //     ContextMenu(column);
            //     column++;
            // }

            ImGui::TableHeadersRow();
            const auto& container = _filter.IsActive() ? _filterContainer : _container;
            _clipper.Begin(static_cast<int>(container.size()));

            while (_clipper.Step()) {
                auto begin = container.rbegin();
                std::ranges::advance(begin, _clipper.DisplayStart);

                auto end = begin;
                std::ranges::advance(end, _clipper.DisplayEnd - _clipper.DisplayStart, _container.rend());

                for (auto& iterator = begin; iterator != end; ++iterator) {
                    ImGui::TableNextRow();
                    const OrderInfoPtrT& tradeInfo_ = iterator->second;
                    ImGui::PushID(tradeInfo_->_uniqueId);
                    Utils::DrawTradeRow(tradeInfo_, _selectedRow, tradeInfo_->_uniqueId);

                    if (_selectedRow == tradeInfo_->_uniqueId and ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
                        if (tradeInfo_->_portfolio == 0 and ImGui::IsKeyPressed(ImGuiKey_F2)) {
                            OrderFormInfoT info{
                                ._uniqueId    = tradeInfo_->_uniqueId,
                                ._price       = tradeInfo_->_price,
                                ._quantity    = (int)tradeInfo_->_quantity,
                                ._lotSize     = (int)Lancelot::ContractInfo::GetLotSize(tradeInfo_->_token),
                                ._orderNumber = tradeInfo_->_orderNumber,
                                ._type        = OrderType_LIMIT,
                                ._side        = tradeInfo_->_side,
                                ._status      = OrderStatus_REPLACED,
                                ._contract    = Lancelot::ContractInfo::GetDescription(tradeInfo_->_token),
                                ._client      = "PRO",
                                ._marketWatch = ContractInfo::GetLiveDataRef(tradeInfo_->_token),
                            };
                            _manualOrder->Update(info);
                            ImGui::OpenPopup(MODIFY_ORDER_WINDOW);
                        }
                        _manualOrder->Paint(MODIFY_ORDER_WINDOW);

                        if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
                            OrderHistory::Instance().LoadOrderHistory(tradeInfo_->_orderNumber);
                            ImGui::OpenPopup(ORDER_HISTORY_POPUP_WINDOW);
                        }
                        OrderHistory::Instance().Paint(nullptr);

                        if (tradeInfo_->_portfolio == 0 and ImGui::IsKeyPressed(ImGuiKey_Delete)) {
                            _strand.post([&]() {
                                _function(tradeInfo_);
                            });
                        }
                    }
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
            ImGui::Separator();
            if (ImGui::Button("Cancel All")) {
                _cancelOrder.clear();
                for (const auto& value : _container) {
                    if (value.second->_portfolio == 0) {
                        _cancelOrder.push_back(value.second);
                    }
                }
                _closeCancelPopup = true;
                ImGui::OpenPopup(CancelAllOrderWindow);
            }
            DrawManualOrderRequestedForCancel();
            ImGui::SameLine();
            ImGui::Text("| Total : [%zu] |", _container.size());
            ImGui::SameLine();
            ImGui::TextColored(BuySellColor(Lancelot::Side_BUY), "| Buy : [%d] |", _buyCount);
            ImGui::SameLine();
            ImGui::TextColored(BuySellColor(Lancelot::Side_SELL), "| Sell : [%d] |", _sellCount);
            ImGui::SameLine();
            if (_filter.Draw("Filter", -FLT_MIN)) {
                _filterContainer.clear();
                for (const auto& item : _container) {
                    if (_filter.PassFilter(item.second->_contract.data())) {
                        _filterContainer.emplace(item.first, item.second);
                    }
                }
            }
        }
    }
    ImGui::End();
}

void OpenOrders::DrawManualOrderRequestedForCancel() {
    if (ImGui::BeginPopupModal(CancelAllOrderWindow, &_closeCancelPopup)) {
        const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginTable(BeginCancelBookTable, BooksColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
            ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible
            for (const auto& name : BookTableColumnName) {
                ImGui::TableSetupColumn(name, TableColumnFlags | ImGuiTableColumnFlags_WidthStretch);
            }
            ImGui::TableHeadersRow();

            _clipper.Begin(static_cast<int>(_cancelOrder.size()));
            while (_clipper.Step()) {
                auto begin = _cancelOrder.begin() + _clipper.DisplayStart;
                auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
                int  i     = _clipper.DisplayStart;
                for (auto iterator = begin; iterator < end; ++iterator, ++i) {
                    ImGui::PushID(i);
                    ImGui::TableNextRow();
                    Utils::DrawTradeRow(*iterator, _selectedRow, -2);
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
        }
        ImGui::Separator();
        if (ImGui::Button(ICON_MD_DONE " Process")) {
            auto _ = std::async(std::launch::async, [&]() {
                for (const auto& tradeInfo : _cancelOrder) {
                    _strand.post([&]() { _function(tradeInfo); });
                }
            });
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_MD_CANCEL " Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        ImGui::Text("These all %zu orders will sent for cancellation", _cancelOrder.size());
        ImGui::EndPopup();
    }
}

void OpenOrders::Update(const OrderInfoPtrT& tradeInfo_, bool insert_) {
    {
        auto iterator = _hashing.find(tradeInfo_->_uniqueId);
        if (iterator != _hashing.end()) {
            if (const auto position = _container.find(iterator->second); position != _container.end()) {
                auto nextLocation = _container.erase(position);
                if (nextLocation != _container.end()) {
                    _selectedRow = static_cast<int>(nextLocation->second->_uniqueId);
                } else if (not _container.empty()) {
                    _selectedRow = static_cast<int>(_container.rbegin()->second->_uniqueId);
                }
                _buyCount -= static_cast<int>(tradeInfo_->_side == Lancelot::Side_BUY);
                _sellCount -= static_cast<int>(tradeInfo_->_side == Lancelot::Side_SELL);
            }
        }
        _hashing[tradeInfo_->_uniqueId] = tradeInfo_->_time;
    }

    if (insert_) {
        _container.emplace(tradeInfo_->_time, tradeInfo_);
        _buyCount += static_cast<int>(tradeInfo_->_side == Lancelot::Side_BUY);
        _sellCount += static_cast<int>(tradeInfo_->_side == Lancelot::Side_SELL);
    }
}
void OpenOrders::Insert(const OrderInfoPtrT& tradeInfo_, bool insert_) {
    _pendingOrderUpdate.push(std::make_pair(tradeInfo_, insert_));
}
