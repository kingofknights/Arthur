//
// Created by VIKLOD on 30-04-2023.
//

#include "../include/TradeTracker.hpp"

#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/TableColumnInfo.hpp"

void TradeTracker::paint(bool *show_) {
	_pendingTrackerUpdate.consume_one([this](TradeTrackerItemT &tradeTrackerItem_) { _trackerContainer.push_back(tradeTrackerItem_); });

	if (*show_) {
		DrawTracker(show_);
	}
}
void TradeTracker::DrawTracker(bool *show_) {
	if (ImGui::Begin("Tracker", show_)) {
		const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginTable("Tracker", TradeTrackerColumn_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
			for (const auto name : TradeTrackerColumnName) {
				ImGui::TableSetupColumn(name, TableColumnFlags);
			}
			ImGui::TableHeadersRow();
			_clipper.Begin(_trackerContainer.size());
			while (_clipper.Step()) {
				auto begin = _trackerContainer.begin() + _clipper.DisplayStart;
				auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
				int	 i	   = _clipper.DisplayStart;
				for (auto iterator = begin; iterator < end; ++iterator, ++i) {
					ImGui::TableNextRow();
					TradeTrackerItemT &TradeTrackerItem = *iterator;
					NextCell(TradeTrackerColumn_ID, "%d", i);
					NextCell(TradeTrackerColumn_NAME, "%s", TradeTrackerItem.Strategy.data());
					NextCell(TradeTrackerColumn_DESCRIPTIONS, "%s", TradeTrackerItem.Descriptions.data());
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
void TradeTracker::Insert(TradeTrackerItemT &tradeTrackerItem_) { _pendingTrackerUpdate.push(tradeTrackerItem_); }
