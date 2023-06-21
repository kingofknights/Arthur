//
// Created by VIKLOD on 22-01-2023.
//

#include "../include/MarketWatch.hpp"

#include <nlohmann/json.hpp>

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Colors.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/OrderForm.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Utils.hpp"
extern AllContractT				AllContract;
extern AddContractToDemoSignalT AddContractToDemoSignal;

#define MARKET_WATCH_CONFIG_PATH "Config/MarketWatch.json"
MarketWatch::MarketWatch(const OrderFormPtrT& manualOrder_) : _manualOrderPtr(manualOrder_), _ladderDataPtr(std::make_shared<MarketWatchDataT>()) {
	Imports(MARKET_WATCH_CONFIG_PATH);
}

MarketWatch::~MarketWatch() {
	Exports(MARKET_WATCH_CONFIG_PATH);
}

void MarketWatch::DrawMarketWatchTable(bool* show_) {
	if (_toBeDeleted != -1) {
		Remove();
	}

	if (ImGui::Begin("MarketWatch", show_)) {
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Stock");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 4);
		if (_searchOrDrop) {
			_filter.Draw("##searchbar");
			DrawSearchBox();
		} else {
			if (ImGui::BeginCombo("##AllContracts", _currentContract.data())) {
				_clipper.Begin(AllContract.size());
				while (_clipper.Step()) {
					auto begin = AllContract.begin() + _clipper.DisplayStart;
					auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
					for (auto iterator = begin; iterator < end; ++iterator) {
						if (ImGui::Selectable(iterator->data())) {
							_currentContract = *iterator;
						}
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::SameLine();
		ImGui::Checkbox("Filter", &_searchOrDrop);
		ImGui::SameLine();
		ImGui::BeginDisabled(_currentContract.empty());
		if (ImGui::Button(ICON_MD_ADD_BOX " Add Stock")) {
			addContractToMarketWatch(_currentContract);
			_filter.Clear();
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_ALL_INCLUSIVE " Future")) {
			for (const auto& contract : AllContract) {
				if (contract.starts_with("FUT")) {
					addContractToMarketWatch(contract);
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MD_CLEAR_ALL " Clear All")) {
			_liveUpdates.clear();
			_subscribed.clear();
		}
		ImGui::SameLine();
		ImGui::Text("Subscribed: %zu", _subscribed.size());

		if (ImGui::BeginTable("Market Watch Table", MarketWatchColumnIndex_END, TableFlags)) {
			ImGui::TableSetupScrollFreeze(1, 0);
			for (const auto& columnName : MarketWatchTableColumnName) {
				ImGui::TableSetupColumn(columnName, TableColumnFlags);
			}
			ImGui::TableHeadersRow();

			_clipper.Begin(_liveUpdates.size());
			while (_clipper.Step()) {
				auto begin = _liveUpdates.begin() + _clipper.DisplayStart;
				auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
				int	 index = _clipper.DisplayStart;
				for (auto iterator = begin; iterator < end; ++iterator) {
					ImGui::TableNextRow();
					DrawColumn(*iterator, index);
					++index;
				}
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void MarketWatch::ToolTipDisplay(const MarketWatchDataPtrT& pointer_) {
	if (ImGui::BeginTooltip()) {
		MarketWatch::LadderView(pointer_);
		ImGui::EndTooltip();
	}
}

void MarketWatch::ContractCell(int contract_, int index_, const char* data_, const MarketWatchDataPtrT& pointer_) {
	FirstCell(index_, data_, _selectedRow, contract_);
	if (_selectedRow == contract_ and ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
		if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
			_toBeDeleted = contract_;
		}

		if (ImGui::BeginPopupContextItem("Context Menu", ImGuiPopupFlags_MouseButtonRight)) {
			if (ImGui::Selectable(ICON_MD_ADD_BOX " Show option chain")) {
				_optionChainContractSignal(data_);
			}

			ImGui::EndPopup();
		}

		bool	 open = false;
		SideType side;
		if (ImGui::IsKeyPressed(ImGuiKey_KeypadAdd) or ImGui::IsKeyPressed(ImGuiKey_F1)) {
			open = true;
			side = Side_BUY;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract) or ImGui::IsKeyPressed(ImGuiKey_F2)) {
			open = true;
			side = Side_SELL;
		}

		if (open) {
			OrderFormInfoT info{.Gateway	   = 0,
								  .Price	   = pointer_->LastTradePrice,
								  .Quantity	   = (int)Lancelot::ContractInfo::GetLotMultiple(pointer_->Token),
								  .LotSize	   = info.Quantity,
								  .OrderNumber = 0,
								  .Type		   = 0,
								  .Side		   = side,
								  .Status	   = OrderStatus_NEW,
								  .Contract	   = Lancelot::ContractInfo::GetDescription(pointer_->Token),
								  .Client	   = "Pro",
								  .Self		   = pointer_};
			_manualOrderPtr->Update(info);
			ImGui::OpenPopup(NEW_ORDER_WINDOW);
		}
		_manualOrderPtr->paint(NEW_ORDER_WINDOW);
	}
	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			_selectedRow   = contract_;
			_ladderDataPtr = pointer_;
		}
		MarketWatch::ToolTipDisplay(pointer_);
	}
}

void MarketWatch::LadderView(const MarketWatchDataPtrT& pointer_) {
	ImGui::LabelText("Contract", "%s", pointer_->Description.data());
	ImGui::Separator();
	if (ImGui::BeginTable("Market Watch Table ToolTip", MarketWatchToolTipColumnIndex_END)) {
		for (const auto& columnName : MarketWatchTableToolTipColumnName) {
			ImGui::TableSetupColumn(columnName, ImGuiTableColumnFlags_WidthStretch);
		}
		ImGui::TableHeadersRow();
		for (int i = 0; i < MARKET_WATCH_LADDER_COUNT; ++i) {
			ImGui::TableNextRow();
			NextCell(MarketWatchToolTipColumnIndex_BUY_ORDER, "%d", pointer_->Bid[i].Order, BuySellColor(Side_BUY));
			NextCell(MarketWatchToolTipColumnIndex_BUY_QUANTITY, "%d", pointer_->Bid[i].Quantity, BuySellColor(Side_BUY));
			NextCell(MarketWatchToolTipColumnIndex_BUY_PRICE, "%.2f", pointer_->Bid[i].Price, BuySellColor(Side_BUY));
			NextCell(MarketWatchToolTipColumnIndex_ASK_PRICE, "%.2f", pointer_->Ask[i].Price, BuySellColor(Side_SELL));
			NextCell(MarketWatchToolTipColumnIndex_ASK_QUANTITY, "%d", pointer_->Ask[i].Quantity, BuySellColor(Side_SELL));
			NextCell(MarketWatchToolTipColumnIndex_ASK_ORDER, "%d", pointer_->Ask[i].Order, BuySellColor(Side_SELL));
		}
		ImGui::EndTable();
	}

	ImGui::Columns(2, nullptr, false);

	ImGui::LabelText("Open", "%.2f", pointer_->OpenPrice);
	ImGui::LabelText("Low", "%.2f", pointer_->LowPrice);
	ImGui::LabelText("LTP", "%.2f", pointer_->LastTradePrice);

	ImGui::NextColumn();
	ImGui::LabelText("High", "%.2f", pointer_->HighPrice);
	ImGui::LabelText("Close", "%.2f", pointer_->ClosePrice);
	ImGui::LabelText("ATP", "%.2f", pointer_->AverageTradePrice);

	ImGui::EndColumns();

	auto  range	 = (pointer_->HighPrice - pointer_->LowPrice);
	float moment = float(pointer_->LastTradePrice - pointer_->LowPrice) / float(range == 0 ? 1 : range);

	uint64_t total = (pointer_->TotalBuyQuantity + pointer_->TotalSellQuantity);
	float	 ratio = float(pointer_->TotalBuyQuantity) / float(total == 0 ? 1 : total);

	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, BuySellColor(Side_BUY));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, BuySellColor(Side_SELL));

	ImGui::ProgressBar(moment, ImVec2(-FLT_MIN, 0), "Price Movement");
	ImGui::ProgressBar(ratio, ImVec2(-FLT_MIN, 0), "Buy Sell Ratio");

	ImGui::PopStyleColor(2);
}

void MarketWatch::Connect(OptionChainContractSlot callback_) {
	_optionChainContractSignal.connect(callback_);
}

void MarketWatch::addContractToMarketWatch(const std::string& contract_) {
	auto token = Lancelot::ContractInfo::GetToken(contract_);
	if (not _subscribed.contains(token)) {
		auto ref = ContractInfo::GetLiveDataRef(token);
		if (not ref) return;
		_liveUpdates.push_back(ref);
		_subscribed.emplace(token);
		AddContractToDemoSignal(token);
	}
}

void MarketWatch::paint(bool* showMarketWatch_, bool* showLadder_) {
	if (*showMarketWatch_) {
		DrawMarketWatchTable(showMarketWatch_);
	}
	if (*showLadder_) {
		DrawLadderWatchWindow(showLadder_);
	}
}

void MarketWatch::DrawLadderWatchWindow(bool* show_) {
	if (ImGui::Begin("MarketWatchLadder", show_)) {
		if (_ladderDataPtr) {
			MarketWatch::LadderView(_ladderDataPtr);
		}
	}
	ImGui::End();
}

void MarketWatch::Imports(std::string_view path_) {
	std::fstream file(path_.data(), std::ios::in);
	if (not file.is_open()) return;

	nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);
	std::ranges::for_each(root.items(), [&](auto& value_) { addContractToMarketWatch(value_.key()); });

	file.close();
}

void MarketWatch::Exports(std::string_view path_) {
	if (_subscribed.empty()) {
		std::remove(path_.data());
		return;
	}
	nlohmann::ordered_json root;

	for (const LiveContainerT::value_type& valueType_ : _liveUpdates) {
		root[valueType_->Description.data()] = valueType_->Token;
	}

	std::fstream file(path_.data(), std::ios::out | std::ios::trunc);
	if (not file.is_open()) return;
	file << root.dump();
	file.close();
}

void MarketWatch::DrawColumn(const MarketWatchDataPtrT& data, int index_) {
	ContractCell(index_, MarketWatchColumnIndex_CONTACT_NAME, data->Description.data(), data);
	NextCell(MarketWatchColumnIndex_ATP, "%.2f", data->AverageTradePrice, UpDownColor(data->Color.ATP));
	NextCell(MarketWatchColumnIndex_LTP, "%.2f", data->LastTradePrice, UpDownColor(data->Color.LTP));
	NextCell(MarketWatchColumnIndex_LTQ, "%d", data->LastTradeQuantity);
	NextCell(MarketWatchColumnIndex_LTT, "%s", data->LastTradeTime.data());
	NextCell(MarketWatchColumnIndex_TOP_BID, "%.2f", data->Bid[0].Price, UpDownColor(data->Color.TopBid));
	NextCell(MarketWatchColumnIndex_TOP_ASK, "%.2f", data->Ask[0].Price, UpDownColor(data->Color.TopAsk));
	NextCell(MarketWatchColumnIndex_OPEN, "%.2f", data->OpenPrice);
	NextCell(MarketWatchColumnIndex_HIGH, "%.2f", data->HighPrice);
	NextCell(MarketWatchColumnIndex_LOW, "%.2f", data->LowPrice);
	NextCell(MarketWatchColumnIndex_CLOSE, "%.2f", data->ClosePrice);
	NextCell(MarketWatchColumnIndex_LOWDPR, "%.2f", data->LowDPR);
	NextCell(MarketWatchColumnIndex_HIGHDPR, "%.2f", data->HighDPR);
	NextCell(MarketWatchColumnIndex_TOTAL_BUY_QUANTITY, "%llu", data->TotalBuyQuantity);
	NextCell(MarketWatchColumnIndex_TOTAL_SELL_QUANTITY, "%llu", data->TotalSellQuantity);
	NextCell(MarketWatchColumnIndex_VOLUME_TRADED_TODAY, "%llu", data->VolumeTradedToday);
	NextCell(MarketWatchColumnIndex_OPEN_INTEREST, "%llu", data->OpenInterest);
}

void MarketWatch::Remove() {
	_subscribed.erase(_ladderDataPtr->Token);
	auto iterator = _liveUpdates.erase(_liveUpdates.begin() + _toBeDeleted);

	_toBeDeleted = -1;

	if (iterator != _liveUpdates.end()) {
		_selectedRow   = std::distance(_liveUpdates.begin(), iterator);
		_ladderDataPtr = *iterator;
	} else if (not _liveUpdates.empty()) {
		--iterator;
		_selectedRow   = std::distance(_liveUpdates.begin(), iterator);
		_ladderDataPtr = *iterator;
	} else {
		_ladderDataPtr.reset();
	}
}

void MarketWatch::DrawSearchBox() {
	if (_filter.IsActive()) {
		ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
		if (ImGui::Begin("Contract Filter", nullptr, OverlayFlags)) {
			Utils::ContractFilter(_filter, _currentContract);
		}
		ImGui::End();
	}
}
