#include "../include/OpenOrders.hpp"

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Colors.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/OrderForm.hpp"
#include "../include/OrderHistory.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Utils.hpp"
#include "imgui.h"

#define CANCEL_ALL_ORDER_WINDOW "Cancel All Order Window"

OpenOrders::OpenOrders(const OrderFormPtrT& manualOrder_, boost::asio::io_context::strand& strand_) : _manualOrderPtr(manualOrder_), _strand(strand_) {}

void OpenOrders::paint(bool* show_) {
    _pendingOrderUpdate.consume_one([this](const std::pair<OrderInfoPtrT, bool>& pair_) { Update(pair_.first, pair_.second); });
    if (*show_) {
        DrawPendingBook(show_);
    }
}

void OpenOrders::DrawPendingBook(bool* show_) {
    if (ImGui::Begin("Open Orders", show_)) {
        const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginTable("Open Orders Table", BooksColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
            for (const auto& name : BookTableColumnName) {
                ImGui::TableSetupColumn(name, TableColumnFlags);
            }

            ImGui::TableHeadersRow();
            _clipper.Begin(_container.size());
            while (_clipper.Step()) {
                auto begin = _container.rbegin();
                std::ranges::advance(begin, _clipper.DisplayStart);

                auto end = begin;
                std::ranges::advance(end, _clipper.DisplayEnd - _clipper.DisplayStart, _container.rend());

                for (auto& iterator = begin; iterator != end; ++iterator) {
                    ImGui::TableNextRow();
                    const OrderInfoPtrT& tradeInfo_ = iterator->second;
                    ImGui::PushID(tradeInfo_->_uniqueId);
                    Utils::DrawTradeRow(tradeInfo_, _selectedRow, tradeInfo_->_uniqueId);

                    if (_selectedRow == tradeInfo_->_uniqueId and ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
                        if (tradeInfo_->_portfolio % 10000 and ImGui::IsKeyPressed(ImGuiKey_F2)) {
                            OrderFormInfoT info{ ._uniqueId    = tradeInfo_->_uniqueId,
                                                 ._price       = tradeInfo_->_price,
                                                 ._quantity    = (int)tradeInfo_->_quantity,
                                                 ._lotSize     = (int)Lancelot::ContractInfo::GetLotMultiple(tradeInfo_->_token),
                                                 ._orderNumber = tradeInfo_->_orderNumber,
                                                 ._type        = 0,
                                                 ._side        = tradeInfo_->_side,
                                                 ._status      = OrderStatus_REPLACED,
                                                 ._contract    = Lancelot::ContractInfo::GetDescription(tradeInfo_->_token),
                                                 ._client      = "PRO",
                                                 ._marketWatch = ContractInfo::GetLiveDataRef(tradeInfo_->_token) };
                            _manualOrderPtr->Update(info);
                            ImGui::OpenPopup(MODIFY_ORDER_WINDOW);
                        }
                        _manualOrderPtr->paint(MODIFY_ORDER_WINDOW);

                        if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
                            OrderHistory::Instance().LoadOrderHistory(tradeInfo_->_orderNumber);
                            ImGui::OpenPopup(ORDER_HISTORY_POPUP_WINDOW);
                        }
                        OrderHistory::Instance().paint(nullptr);

                        if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
                            _strand.post([&]() { _cancelPendingOrderFunction(tradeInfo_); });
                        }
                    }
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();
            ImGui::Separator();
            if (ImGui::Button("Cancel All")) {
                _cancelOrder.clear();
                for (const PendingBookContainerT::value_type& value : _container) {
                    if (value.second->_portfolio % 10000 == 9999) {
                        _cancelOrder.push_back(value.second);
                    }
                }
                _closeCancelPopup = true;
                ImGui::OpenPopup(CANCEL_ALL_ORDER_WINDOW);
            }
            DrawManualOrderRequestedForCancel();
            ImGui::SameLine();
            ImGui::Text("| Total : [%zu] |", _container.size());
            ImGui::SameLine();
            ImGui::TextColored(BuySellColor(Lancelot::Side_BUY), "| Buy : [%d] |", _buyCount);
            ImGui::SameLine();
            ImGui::TextColored(BuySellColor(Lancelot::Side_SELL), "| Sell : [%d] |", _sellCount);
        }
    }
    ImGui::End();
}

void OpenOrders::DrawManualOrderRequestedForCancel() {
    if (ImGui::BeginPopupModal(CANCEL_ALL_ORDER_WINDOW, &_closeCancelPopup)) {
        const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginTable("Cancel Book Table", BooksColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
            for (const auto& name : BookTableColumnName) {
                ImGui::TableSetupColumn(name, TableColumnFlags | ImGuiTableColumnFlags_WidthStretch);
            }
            ImGui::TableHeadersRow();

            _clipper.Begin(_cancelOrder.size());
            while (_clipper.Step()) {
                auto begin = _cancelOrder.begin() + _clipper.DisplayStart;
                auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
                int  i     = _clipper.DisplayStart;
                ImGui::PushID(i);
                for (auto iterator = begin; iterator < end; ++iterator, ++i) {
                    ImGui::TableNextRow();
                    Utils::DrawTradeRow(*iterator, _selectedRow, -2);
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::Separator();
        if (ImGui::Button(ICON_MD_DONE " Process")) {
            auto _ = std::async(std::launch::async, [&]() {
                for (const auto& tradeInfo : _cancelOrder) {
                    _strand.post([&]() { _cancelPendingOrderFunction(tradeInfo); });
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
            if (_container.erase(iterator->second)) {
                _buyCount -= tradeInfo_->_side == Lancelot::Side_BUY;
                _sellCount -= tradeInfo_->_side == Lancelot::Side_SELL;
            }
        }
        _hashing[tradeInfo_->_uniqueId] = tradeInfo_->_time;
    }

    if (insert_) {
        auto success = _container.emplace(tradeInfo_->_time, tradeInfo_).second;
        _buyCount += tradeInfo_->_side == Lancelot::Side_BUY;
        _sellCount += tradeInfo_->_side == Lancelot::Side_SELL;
    }
}
void OpenOrders::Insert(const OrderInfoPtrT& tradeInfo_, bool insert_) {
    _pendingOrderUpdate.push(std::make_pair(tradeInfo_, insert_));
}

void OpenOrders::cancelOrderFunctionCallback(CancelPendingOrderFunctionT cancelPendingOrderFunction_) {
    _cancelPendingOrderFunction = std::move(cancelPendingOrderFunction_);
}
