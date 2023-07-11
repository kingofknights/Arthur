#include "../include/OptionChain.hpp"

#include <Greeks/Greeks.hpp>

#include "../API/ContractInfo.hpp"
#include "../include/Colors.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/Signal.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"

extern AddContractToDemoSignalT AddContractToDemoSignal;

std::string FormatTimeToString(time_t rawtime, std::string Format);

OptionChain::OptionChain() : _future(std::make_shared<MarketWatchDataT>()) {}

void OptionChain::paint(bool* show_) {
	if (*show_) {
		DrawOptionChain(show_);
	}
}

void OptionChain::DrawOptionChain(bool* show_) {
	if (ImGui::Begin("Option Chain", show_)) {
		ImGui::Columns(8, nullptr, false);
		ImGui::Text("Contract : %s", _symbol.data());
		ImGui::NextColumn();
		ImGui::Text("Expiry : %s", _expiry.data());
		ImGui::NextColumn();
		ImGui::TextColored(UpDownColor(_future->Color.LTP), "LTP : %.2f", _future->LastTradePrice);
		ImGui::NextColumn();
		ImGui::TextColored(UpDownColor(_future->PercentageChange), "Change : %.2f", _future->PercentageChange);
		ImGui::NextColumn();
		ImGui::Text("Bid Qty : %d", _future->Bid[0].Quantity);
		ImGui::NextColumn();
		ImGui::TextColored(UpDownColor(_future->Color.TopBid), "Bid Price : %.2f", _future->Bid[0].Price);
		ImGui::NextColumn();
		ImGui::TextColored(UpDownColor(_future->Color.TopAsk), "Ask Price : %.2f", _future->Ask[0].Price);
		ImGui::NextColumn();
		ImGui::Text("Ask Qty : %d", _future->Ask[0].Quantity);
		ImGui::EndColumns();

		if (ImGui::BeginTable("Option Chain Table", OptionChainColumnIndex_END, TableFlags)) {
			for (const auto& name : OptionChainTableColumnName) {
				ImGui::TableSetupColumn(name, TableColumnFlags | ImGuiTableColumnFlags_NoReorder);
			}
			ImGui::TableHeadersRow();

			for (const OptionChainContainerT::value_type& valueType_ : _OptionChainContainer) {
				ImGui::TableNextRow();

				const OptionChainRowT&	optionChainRow = valueType_.second;
				const OptionChainItemT& Call		   = optionChainRow.Call;
				const OptionChainItemT& Put			   = optionChainRow.Put;

				float priceForCall = _future->Bid[0].Price ? _future->Bid[0].Price : _future->LastTradePrice;
				// FIXME : remove abs when working with live contracts
				float expiryGap	 = std::abs(Greeks::GetExpiryGap(Put.ResultSet->_expiryDate));
				float rate		 = 0.0f;
				float call_IV	 = Greeks::GetIV(priceForCall, valueType_.first, rate, expiryGap, Call.Self->LastTradePrice, true);
				float call_Theta = Greeks::GetTheta(priceForCall, valueType_.first, call_IV, rate, expiryGap, true);
				float call_Vega	 = Greeks::GetVega(priceForCall, valueType_.first, call_IV, rate, expiryGap, true);
				float call_Gamma = Greeks::GetGamma(priceForCall, valueType_.first, call_IV, rate, expiryGap, true);
				float call_Delta = Greeks::GetDelta(priceForCall, valueType_.first, call_IV, rate, expiryGap, true);

				float priceForPut = _future->Ask[0].Price ? _future->Ask[0].Price : _future->LastTradePrice;
				float put_IV	  = Greeks::GetIV(priceForPut, valueType_.first, rate, expiryGap, Put.Self->LastTradePrice, false);
				float put_Theta	  = Greeks::GetTheta(priceForPut, valueType_.first, put_IV, rate, expiryGap, false);
				float put_Vega	  = Greeks::GetVega(priceForPut, valueType_.first, put_IV, rate, expiryGap, false);
				float put_Gamma	  = Greeks::GetGamma(priceForPut, valueType_.first, put_IV, rate, expiryGap, false);
				float put_Delta	  = Greeks::GetDelta(priceForPut, valueType_.first, put_IV, rate, expiryGap, false);

				ImU32 color		= ImGui::GetColorU32(COLOR_GRAY);
				bool  needColor = valueType_.first < _future->LastTradePrice;

				NextCell(OptionChainColumnIndex_CALL_OI, "%d", Call.Self->OpenInterest);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_VOLUME, "%d", Call.Self->VolumeTradedToday);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_DELTA, "%.2f", call_Delta);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_GAMMA, "%.2f", call_Gamma);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_VEGA, "%.2f", call_Vega);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_THETA, "%.2f", call_Theta);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}

				NextCell(OptionChainColumnIndex_CALL_IV, "%.2f", call_IV * 100.0f);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_LTP, "%.2f", Call.Self->LastTradePrice, UpDownColor(Call.Self->Color.LTP));
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_CHANGE, "%.2f", Call.Self->PercentageChange);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_BID_QTY, "%d", Call.Self->Bid[0].Quantity);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_BID_PRICE, "%.2f", Call.Self->Bid[0].Price, UpDownColor(Call.Self->Color.TopBid));
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_ASK_PRICE, "%.2f", Call.Self->Ask[0].Price, UpDownColor(Call.Self->Color.TopAsk));
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_CALL_ASK_QTY, "%d", Call.Self->Ask[0].Quantity);
				if (needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}

				NextCell(OptionChainColumnIndex_STRIKE_PRICE, "%.2f", valueType_.first);

				NextCell(OptionChainColumnIndex_PUT_BID_QTY, "%d", Put.Self->Bid[0].Quantity);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_PUT_BID_PRICE, "%.2f", Put.Self->Bid[0].Price, UpDownColor(Put.Self->Color.TopBid));
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_PUT_ASK_PRICE, "%.2f", Put.Self->Ask[0].Price, UpDownColor(Put.Self->Color.TopAsk));
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_PUT_ASK_QTY, "%d", Put.Self->Ask[0].Quantity);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_PUT_CHANGE, "%.2f", Put.Self->PercentageChange);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_PUT_LTP, "%.2f", Put.Self->LastTradePrice, UpDownColor(Put.Self->Color.LTP));
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_PUT_IV, "%.2f", put_IV * 100.0f);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}

				NextCell(OptionChainColumnIndex_PUT_THETA, "%.2f", put_Theta);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}

				NextCell(OptionChainColumnIndex_PUT_VEGA, "%.2f", put_Vega);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}

				NextCell(OptionChainColumnIndex_PUT_GAMMA, "%.2f", put_Gamma);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_PUT_DELTA, "%.2f", put_Delta);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}

				NextCell(OptionChainColumnIndex_PUT_VOLUME, "%ld", Put.Self->VolumeTradedToday);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
				NextCell(OptionChainColumnIndex_PUT_OI, "%d", Put.Self->OpenInterest);
				if (not needColor) {
					ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
				}
			}

			ImGui::EndTable();
		}
	}
	ImGui::End();
}
void OptionChain::SetOptionForFuture(const std::string& contract_) {
	int		 token_	 = Lancelot::ContractInfo::GetToken(contract_);
	uint32_t expiry_ = Lancelot::ContractInfo::GetExpiryDate(token_);

	if (not Lancelot::ContractInfo::IsFuture(token_)) {
		token_ = Lancelot::ContractInfo::GetFuture(token_);
	}
	auto future = ContractInfo::GetLiveDataRef(token_);

	if (!future) return;
	_OptionChainContainer.clear();

	_future = future;

	AddContractToDemoSignal(future->Token);
	uint32_t futurePrice_ = future->LastTradePrice * 100.0;
	_symbol				  = Lancelot::ContractInfo::GetSymbol(token_);
	_expiry				  = FormatTimeToString(expiry_, "%d-%m-%Y");

	LoadOptions(_symbol, expiry_, futurePrice_, '>', "asc");
	LoadOptions(_symbol, expiry_, futurePrice_, '<', "desc");
}
void OptionChain::LoadOptions(const std::string& symbol_, uint32_t expiry_, uint32_t futurePrice_, char comparator_, const std::string& order_) {
	auto query = FORMAT(GetOptionChain_, symbol_, symbol_, expiry_, comparator_, futurePrice_, order_);
	auto table = Lancelot::ContractInfo::GetResultWithIndex(query);

	for (const auto& row : table) {
		auto			 callToken = std::stoi(row[0]);
		auto			 putToken  = std::stoi(row[1]);
		OptionChainItemT call{.Self = ContractInfo::GetLiveDataRef(callToken), .ResultSet = Lancelot::ContractInfo::GetResultSet(callToken)};
		OptionChainItemT put{.Self = ContractInfo::GetLiveDataRef(putToken), .ResultSet = Lancelot::ContractInfo::GetResultSet(putToken)};
		OptionChainRowT	 optionChainRow{.Call = call, .Put = put};

		_OptionChainContainer.emplace(std::stof(row[2]) / call.ResultSet->_divisor, optionChainRow);
		AddContractToDemoSignal(callToken);
		AddContractToDemoSignal(putToken);
	}
}
