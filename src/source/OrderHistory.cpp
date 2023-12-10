#include "../include/OrderHistory.hpp"

#include "../include/Colors.hpp"
#include "../include/ConfigLoader.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Utils.hpp"

void OrderHistory::DrawOrderHistory() {
    if (ImGui::BeginPopupModal(ORDER_HISTORY_POPUP_WINDOW, &_showOrderHistory, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginTable("OrderHistory", BooksColumnIndex_END, TableFlags)) {
            for (const auto& name : BookTableColumnName) {
                ImGui::TableSetupColumn(name, ImGuiTableColumnFlags_WidthStretch);
            }
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(_container.size());
            while (clipper.Step()) {
                auto begin = _container.begin() + clipper.DisplayStart;
                auto end   = begin + (clipper.DisplayEnd - clipper.DisplayStart);

                for (auto iterator = begin; iterator != end; ++iterator) {
                    ImGui::TableNextRow();
                    Utils::DrawTradeRow(*iterator, _index, -1);
                }
            }
            ImGui::EndTable();
        }

        ImGui::EndPopup();
    }
}

void OrderHistory::paint(bool* show_) {
    if (_showOrderHistory) {
        DrawOrderHistory();
    }
}

void OrderHistory::LoadOrderHistory(double orderNumber_) {
    _showOrderHistory = true;
    _container.clear();
    _container = ConfigLoader::Instance().getOrderHistory(orderNumber_);
}

OrderHistory& OrderHistory::Instance() {
    static OrderHistory orderHistory;
    return orderHistory;
}
