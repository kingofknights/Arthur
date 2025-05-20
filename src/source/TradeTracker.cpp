#include "TradeTracker.hpp"

#include "Configuration.hpp"
#include "Enums.hpp"
#include "TableColumnInfo.hpp"

void TradeTracker::paint(bool* show_) {
    _pendingTrackerUpdate.consume_all([this](TradeTrackerItemT tradeTrackerItem_) { _trackerContainer.push_back(std::move(tradeTrackerItem_)); });

    if (*show_) {
        DrawTracker(show_);
    }
}
void TradeTracker::DrawTracker(bool* show_) {
    if (ImGui::Begin("Tracker", show_)) {
        const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginTable("Tracker", TradeTrackerColumn_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
            for (const auto* const name : TradeTrackerColumnName) {
                ImGui::TableSetupColumn(name, TableColumnFlags);
            }
            ImGui::TableHeadersRow();
            _clipper.Begin(static_cast<int>(_trackerContainer.size()));
            while (_clipper.Step()) {
                auto begin = _trackerContainer.begin() + _clipper.DisplayStart;
                auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
                int  index = _clipper.DisplayStart;
                for (auto iterator = begin; iterator < end; ++iterator, ++index) {
                    ImGui::TableNextRow();
                    TradeTrackerItemT& tradeTrackerItem = *iterator;
                    NextCell(TradeTrackerColumn_ID, index);
                    NextCell(TradeTrackerColumn_NAME, tradeTrackerItem._strategy.data());
                    NextCell(TradeTrackerColumn_DESCRIPTIONS, tradeTrackerItem._descriptions.data());
                }
            }
            ImGui::EndTable();
        }
        ImGui::Separator();
        ImGui::Text("Tracker [%zu]", _trackerContainer.size());
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            _trackerContainer.clear();
        }
    }
    ImGui::End();
}
void TradeTracker::Insert(TradeTrackerItemT& tradeTrackerItem_) { _pendingTrackerUpdate.push(tradeTrackerItem_); }
