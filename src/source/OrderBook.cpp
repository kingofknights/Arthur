//
// Created by VIKLOD on 16-05-2023.
//
#include "../include/OrderBook.hpp"

#include "../include/Colors.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/OrderHistory.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Utils.hpp"

OrderBook::OrderBook(const std::string& name_) : _name(name_), _tableName(name_) { _tableName.append(" Table"); }

void OrderBook::paint(bool* show_) {
	_pendingTradeUpdate.consume_one([this](const OrderInfoPtrT& orderInfo_) { _container.push_back(orderInfo_); });
	if (*show_) {
		DrawOrderBookTable(show_);
	}
}

void OrderBook::Insert(const OrderInfoPtrT& orderInfo_) { _pendingTradeUpdate.push(orderInfo_); }

void OrderBook::DrawOrderBookTable(bool* show_) {
	if (ImGui::Begin(_name.data(), show_)) {
		if (ImGui::BeginTable(_tableName.data(), BooksColumnIndex_END, TableFlags)) {
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
	}
	ImGui::End();
}
