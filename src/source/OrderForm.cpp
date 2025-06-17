#include "OrderForm.hpp"

#include "API/Common.hpp"
#include "Colors.hpp"
#include "ContractInfo.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "MarketWatch.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"
#include "imgui.h"

#include <imgui_internal.h>

#include <algorithm>
#include <utility>

extern ClientCodeListT ClientCodeList;

auto OrderForm::RoundUp(int numToRound_, int multiple_) -> int {
    int value = ((numToRound_ + multiple_ - 1) / multiple_) * multiple_;
    return value;
}

OrderForm::OrderForm(ExecutorStrandT& strand_, FunctionT function_)
    : _strand(strand_),
      _function(std::move(function_)),
      _order{},
      _color(COLOR_GRAY) {}

void OrderForm::Paint(const char* name_) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, _color);
    ImGui::PushStyleColor(ImGuiCol_Border, _color);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    if (ImGui::BeginPopupModal(name_, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        MarketWatch::LadderView(_order._marketWatch);
        ImGui::Separator();
        DrawInputItem();
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
}

void OrderForm::Update(OrderFormInfoT& info_) {
    _resultSet = Lancelot::ContractInfo::GetResultSet(info_._marketWatch->_token);
    _order     = info_;
    _price     = RoundUp(int(info_._price * 100), 5) / 100.0;
    _color     = BuySellColor(_order._side);
    _textColor = _order._side == Lancelot::Side_BUY ? COLOR_BLACK : COLOR_WHITE;
    _precision = static_cast<double>(_resultSet->_tickSize) / static_cast<double>(_resultSet->_divisor);

    auto exchange = _resultSet->_exchange;

    if (exchange != _exchange) {
        _exchange = exchange;
        for (const auto& item : ClientCodeList) {
            if (_exchange == item._exchange) {
                _clientCode   = item._clientCode;
                info_._client = _clientCode;
                break;
            }
        }
    }
}
void OrderForm::SentToBroker() {
    _order._price = static_cast<PriceT>(RoundUp(int(_price * _resultSet->_divisor), static_cast<int>(_resultSet->_tickSize)));
    _strand.post([&]() { _function(_order, _order._orderNumber == 0 ? Lancelot::RequestType_NEW : Lancelot::RequestType_MODIFY); });
}

void OrderForm::DrawInputItem() {
    ImGui::PushStyleColor(ImGuiCol_Text, _textColor);
    if (ImGui::InputDouble("##Price", &_price, _precision, _precision, "%.2f")) {
        _price = std::max(_price, 0.0);
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("Price");

    ImGui::PushStyleColor(ImGuiCol_Text, _textColor);
    if (ImGui::InputInt("##Quantity", &_order._quantity, _order._lotSize)) {
        _order._quantity = std::max(_order._quantity, _order._lotSize);
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("Quantity");

    bool enable = _order._status != OrderStatus_NEW;
    ImGui::BeginDisabled(enable);
    ImGui::PushStyleColor(ImGuiCol_Text, _textColor);
    if (ImGui::BeginCombo("##Broker", FORMAT("[{}] {}", Lancelot::ToString(_exchange), _clientCode).data())) {
        for (const auto& code : ClientCodeList) {
            if (code._exchange == _exchange) {
                if (ImGui::Selectable(FORMAT("[{}] {}", Lancelot::ToString(code._exchange), code._clientCode).data())) {
                    _order._client = code._clientCode;
                    _exchange      = code._exchange;
                    _clientCode    = code._clientCode;
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("Broker");

    ImGui::PushStyleColor(ImGuiCol_Text, _textColor);
    if (ImGui::BeginCombo("##Type", OrderTypeName[_order._type])) {
        for (int type : {OrderType_IOC, OrderType_LIMIT}) {
            if (ImGui::Selectable(OrderTypeName[type])) {
                _order._type = type;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("Type");

    ImGui::EndDisabled();
    ImGui::LabelText("OrderNumber", "%lu", _order._orderNumber);

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        ImGui::CloseCurrentPopup();
    }
    ImGui::Columns(2, nullptr, false);
    if (not enable) {
        ImGui::PushStyleColor(ImGuiCol_Text, _textColor);
        ImGui::PushStyleColor(ImGuiCol_CheckMark, _textColor);
        ImGui::Checkbox("##Multiple Order", &_repeater);
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
        ImGui::Text("Multiple Order");
    }
    if (not _repeater) {
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) or ImGui::IsKeyPressed(ImGuiKey_KeypadEnter) or ImGui::Button(ICON_MD_DONE " Submit", {-FLT_MIN, 0})) {
            SentToBroker();
            if (enable) ImGui::CloseCurrentPopup();
        }
    } else {
        if (ImGui::Button(FORMAT("{} Submit {}", ICON_MD_DONE, _repeaterCount).data(), {-FLT_MIN, 0})) {
            for (int i = 0; i < _repeaterCount; i++) {
                SentToBroker();
            }
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::NextColumn();
    if (not enable) {
        ImGui::BeginDisabled(not _repeater);
        ImGui::PushStyleColor(ImGuiCol_Text, _textColor);
        ImGui::InputInt("##Count", &_repeaterCount);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::Text("Count");
        ImGui::EndDisabled();
    }
    if (ImGui::Button(ICON_MD_CANCEL " Cancel", {-FLT_MIN, 0})) {
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndColumns();
}
