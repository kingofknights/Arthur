#include "../include/OrderForm.hpp"

#include "../API/Common.hpp"
#include "../include/Colors.hpp"
#include "../include/MarketWatch.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "imgui.h"

#include <imgui_internal.h>

#include <algorithm>
#include <utility>

extern ClientCodeListT ClientCodeList;

auto OrderForm::RoundUp(int numToRound_, int multiple_) -> int {
    int value = ((numToRound_ + multiple_ - 1) / multiple_) * multiple_;
    LOG(INFO, "num = {}, multiple_ = {}, value = {}", numToRound_, multiple_, value);
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
    _order        = info_;
    _order._price = RoundUp(int(info_._price * 100), 5) / 100.0;
    _color        = BuySellColor(_order._side);
    auto exchange = Lancelot::ContractInfo::GetExchange(Lancelot::ContractInfo::GetToken(info_._contract));
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
    _order._price = RoundUp(int(_order._price * 100), 5) / 100.0;
    _strand.post([&]() { _function(_order, _order._orderNumber == 0 ? Lancelot::RequestType_NEW : Lancelot::RequestType_MODIFY); });
}

void OrderForm::DrawInputItem() {
    if (ImGui::InputDouble("Price", &_order._price, 0.050000000000F, 0.5000000000F, "%.2f")) {
        _order._price = std::max(_order._price, 0.0);
        LOG(INFO, "{}", _order._price * 100.0);
    }

    if (ImGui::InputInt("Quantity", &_order._quantity, _order._lotSize)) {
        _order._quantity = std::max(_order._quantity, _order._lotSize);
    }
    bool enable = _order._status != OrderStatus_NEW;
    ImGui::BeginDisabled(enable);
    if (ImGui::BeginCombo("Broker", FORMAT("[{}] {}", Lancelot::ToString(_exchange), _clientCode).data())) {
        for (const auto& code : ClientCodeList) {
            if (ImGui::Selectable(FORMAT("[{}] {}", Lancelot::ToString(code._exchange), code._clientCode).data())) {
                _order._client = code._clientCode;
                _exchange      = code._exchange;
                _clientCode    = code._clientCode;
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::BeginCombo("Type", OrderTypeName[_order._type])) {
        for (int i = 0; i < 4; i++) {
            if (ImGui::Selectable(OrderTypeName[i])) {
                _order._type = i;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::EndDisabled();
    ImGui::LabelText("OrderNumber", "%ld", _order._orderNumber);

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::Columns(2, nullptr, false);
    if (ImGui::IsKeyPressed(ImGuiKey_Enter) or ImGui::IsKeyPressed(ImGuiKey_KeypadEnter) or ImGui::Button(ICON_MD_DONE " Submit", {-FLT_MIN, 0})) {
        SentToBroker();
        if (enable) ImGui::CloseCurrentPopup();
    }

    ImGui::NextColumn();
    if (ImGui::Button(ICON_MD_CANCEL " Cancel", {-FLT_MIN, 0})) {
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndColumns();
}
