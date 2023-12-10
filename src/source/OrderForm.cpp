#include "../include/OrderForm.hpp"

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Colors.hpp"
#include "../include/Enums.hpp"
#include "../include/MarketWatch.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Utils.hpp"

extern ClientCodeListT ClientCodeList;

OrderForm::OrderForm(boost::asio::io_context::strand& strand_) : _order{}, _color(COLOR_GRAY), _strand(strand_) {}

void OrderForm::paint(const char* name_) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, _color);
    ImGui::PushStyleColor(ImGuiCol_Border, _color);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(name_, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        MarketWatch::LadderView(_order.Self);
        ImGui::Separator();
        DrawInputItem();
        ImGui::EndPopup();
    }

    ImGui::PopStyleColor(2);
}

void OrderForm::Update(OrderFormInfoT& info_) {
    _order        = info_;
    _color        = BuySellColor(_order.Side);
    auto exchange = Lancelot::ContractInfo::GetExchange(Lancelot::ContractInfo::GetToken(info_.Contract));
    if (exchange != _exchange) {
        _exchange = exchange;
        for (const auto& item : ClientCodeList) {
            if (_exchange == item.Exchange) {
                _clientCode  = item.ClientCode;
                info_.Client = _clientCode;
                break;
            }
        }
    }
}
void OrderForm::SentToBroker() {
    _strand.post([&]() { _publishOrderFunction(_order, _order.OrderNumber == 0 ? Lancelot::RequestType_NEW : Lancelot::RequestType_MODIFY); });
}

void OrderForm::DrawInputItem() {
    if (ImGui::InputDouble("Price", &_order.Price, 0.050000000000f, 0.5000000000f, "%.2f")) {
        if (_order.Price < 0) _order.Price = 0;
    }
    if (ImGui::InputInt("Quantity", &_order.Quantity, _order.LotSize)) {
        if (_order.Quantity < _order.LotSize) {
            _order.Quantity = _order.LotSize;
        }
    }
    bool enable = _order.Status != OrderStatus_NEW;
    ImGui::BeginDisabled(enable);
    if (ImGui::BeginCombo("Broker", FORMAT("[{}] {}", Lancelot::toString(_exchange), _clientCode).data())) {
        for (const auto& code_ : ClientCodeList) {
            if (ImGui::Selectable(FORMAT("[{}] {}", Lancelot::toString(code_.Exchange), code_.ClientCode).data())) {
                _order.Client = code_.ClientCode;
                _exchange     = code_.Exchange;
                _clientCode   = code_.ClientCode;
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::BeginCombo("Type", OrderTypeName[_order.Type])) {
        for (int i = 0; i < 4; i++) {
            if (ImGui::Selectable(OrderTypeName[i])) {
                _order.Type = i;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::EndDisabled();
    ImGui::LabelText("OrderNumber", "%ld", _order.OrderNumber);

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::Columns(2, nullptr, false);
    if (ImGui::IsKeyPressed(ImGuiKey_Enter) or ImGui::IsKeyPressed(ImGuiKey_KeypadEnter) or ImGui::Button(ICON_MD_DONE " Submit", { -FLT_MIN, 0 })) {
        SentToBroker();
        if (enable) ImGui::CloseCurrentPopup();
    }

    ImGui::NextColumn();
    if (ImGui::Button(ICON_MD_CANCEL " Cancel", { -FLT_MIN, 0 })) {
        ImGui::CloseCurrentPopup();
    }
}
void OrderForm::publishOrderCallback(PublishOrderFunctionT publishOrderFunction_) {
    _publishOrderFunction = std::move(publishOrderFunction_);
}
