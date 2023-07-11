#include "../include/TradeHistory.hpp"

#include "../API/Common.hpp"
#include "../include/Colors.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/OrderHistory.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Utils.hpp"

void TradeHistory::paint(bool* show_) {
    _pendingTradeUpdate.consume_one([this](const OrderInfoPtrT& orderInfo_) {
        _container.push_back(orderInfo_);
        _totalBuy += orderInfo_->Side == Lancelot::Side_BUY;
        _totalSell += orderInfo_->Side == Lancelot::Side_SELL;
        _buyValue += orderInfo_->Side == Lancelot::Side_BUY ? orderInfo_->Price * orderInfo_->Quantity : 0;
        _sellValue += orderInfo_->Side == Lancelot::Side_SELL ? orderInfo_->Price * orderInfo_->Quantity : 0;
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
	if (ImGui::Begin("Trade Book", show_)) {
		const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginTable("Trade Book Table", BooksColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
			for (const auto& name : BookTableColumnName) {
				ImGui::TableSetupColumn(name, TableColumnFlags);
			}
			ImGui::TableHeadersRow();

			_clipper.Begin(_container.size());
			while (_clipper.Step()) {
				auto begin = _container.rbegin() + _clipper.DisplayStart;
				auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
				int	 i	   = _clipper.DisplayStart;
				for (auto iterator = begin; iterator < end; ++iterator, ++i) {
					ImGui::TableNextRow();
					const OrderInfoPtrT& tradeInfo_ = *iterator;
					ImGui::PushID(i);
					Utils::DrawTradeRow(tradeInfo_, _selectedRow, i);
					if (_selectedRow == i) {
						if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
							OrderHistory::Instance().LoadOrderHistory(tradeInfo_->OrderNo);
							ImGui::OpenPopup(ORDER_HISTORY_POPUP_WINDOW);
						}
						OrderHistory::Instance().paint(nullptr);
					}
					ImGui::PopID();
				}
			}

			ImGui::EndTable();
		}
		ImGui::Separator();
		ImGui::Text("| Total : [%zu] |", _container.size());
		ImGui::SameLine();
		ImGui::TextColored(UpDownColor(1), "| Buy : [%d] |", _totalBuy);
		ImGui::SameLine();
		ImGui::TextColored(UpDownColor(1), "| Buy Value : [%.2f] |", _buyValue);
		ImGui::SameLine();
		ImGui::TextColored(UpDownColor(-1), "| Sell : [%d] |", _totalSell);
		ImGui::SameLine();
		ImGui::TextColored(UpDownColor(-1), "| Sell Value : [%.2f] |", _sellValue);
		ImGui::SameLine();
		ImGui::TextColored(UpDownColor(_netValue), "| Net Value : [%.2f] |", _netValue);
	}
	ImGui::End();
}
