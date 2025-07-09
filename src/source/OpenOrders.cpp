#include "OpenOrders.hpp"

#include "API/Common.hpp"
#include "API/ContractInfo.hpp"
#include "Arthur_Fwd.hpp"
#include "Colors.hpp"
#include "Configuration.hpp"
#include "Enums.hpp"
#include "IconsMaterialDesign.h"
#include "Logger.hpp"
#include "OrderForm.hpp"
#include "OrderHistory.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"
#include "Utils.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <cfloat>
#include <iterator>
#include <string>

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

void OpenOrders::DoFilter(const OrderInfoPtrT& tradeInfo_) {
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
    _filterContainer.emplace(tradeInfo_->_time, tradeInfo_);
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
            /*
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
            */

            const auto& container = _isFilterActive ? _filterContainer : _container;
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
                ImGui::OpenPopup(CancelAllOrderWindow);
            }
            DrawManualOrderRequestedForCancel();
            ImGui::SameLine();
            ImGui::Text("| Total : [%zu] |", _container.size());
            ImGui::SameLine();
            ImGui::TextColored(BuySellColor(Lancelot::Side_BUY), "| Buy : [%d] |", _buyCount);
            ImGui::SameLine();
            ImGui::TextColored(BuySellColor(Lancelot::Side_SELL), "| Sell : [%d] |", _sellCount);
            if (_isFilterActive) {
                ImGui::SameLine();
                ImGui::Text("%s", "Filter is active");
            }
        }
    }
    ImGui::End();
}

void OpenOrders::DrawManualOrderRequestedForCancel() {
    if (ImGui::BeginPopupModal(CancelAllOrderWindow)) {
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

            _filterContainer.erase(iterator->second);
        }
        _hashing[tradeInfo_->_uniqueId] = tradeInfo_->_time;
    }

    if (insert_) {
        _container.emplace(tradeInfo_->_time, tradeInfo_);
        _buyCount += static_cast<int>(tradeInfo_->_side == Lancelot::Side_BUY);
        _sellCount += static_cast<int>(tradeInfo_->_side == Lancelot::Side_SELL);

        if (_isFilterActive) {
            if (Utils::CheckPassFiler(tradeInfo_, _filter, _active)) {
                _filterContainer.emplace(tradeInfo_->_time, tradeInfo_);
            }
        }
    }
}
void OpenOrders::Insert(const OrderInfoPtrT& tradeInfo_, bool insert_) {
    _pendingOrderUpdate.push(std::make_pair(tradeInfo_, insert_));
}

void OpenOrders::FilterOptionsWindows() {
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

void OpenOrders::StartNewFilter() {
    _isFilterActive = IsFilterActive();
    _filterContainer.clear();
    if (not _isFilterActive) {
        return;
    }

    for (const auto& order : _container) {
        if (Utils::CheckPassFiler(order.second, _filter, _active)) {
            _filterContainer.emplace(order.first, order.second);
        }
    }
}

auto OpenOrders::IsFilterActive() -> bool {
    std::ranges::transform(_filter, _active.begin(), [](const auto& container_) {
        return std::ranges::any_of(container_, [](const auto& pair_) {
            return pair_.second;
        });
    });
    return std::ranges::any_of(_active, [](bool value_) {
        return value_;
    });
}
void OpenOrders::FillFilterOption() {
    if (not _isFilterActive) {
        std::ranges::for_each(_filter, [](auto& item_) { item_.clear(); });
    } else {
        std::ranges::for_each(_filter, [](auto& item_) {
            std::erase_if(item_, [](const auto& pair_) { return !pair_.second; });
        });
    }
    const auto& local = _isFilterActive ? _filterContainer : _container;
    std::ranges::for_each(local, [&](const auto& pair_) {
        const OrderInfoPtrT& order = pair_.second;
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
