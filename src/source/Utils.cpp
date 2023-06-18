//
// Created by VIKLOD on 07-03-2023.
//

#include "../include/Utils.hpp"

#include <filesystem>
#include <future>
#include <nlohmann/json.hpp>

#include "../API/BaseScanner.hpp"
#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"

extern std::string				StatusDisplay;
extern AllContractT				AllContract;
extern ClientCodeListT			ClientCodeList;
extern MarketWatchDatContainerT MarketWatchDatContainer;
extern int						Id;

GlobalStrategyListT Utils::GlobalStrategyList;

std::string Utils::FormatTimeToString(uint64_t time_) {
	char		 timestamp[50] = "";
	time_t		 secs		   = (time_ / 10000000000) + 315513000;
	tm*			 ptm		   = localtime(&secs);
	size_t		 len		   = strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", ptm);
	unsigned int ms			   = time_ % 1000000000;
	sprintf(timestamp + len, ".%09u", ms);
	return timestamp;
}
std::string Utils::manualSerialize(const ManualOrderInfoT& manualOrderInfo_) {
	nlohmann::json json;
	json[JSON_ID] = ++Id;

	nlohmann::json params;
	if (manualOrderInfo_.OrderNumber == 0) {
		params[JSON_TOKEN]		= Lancelot::ContractInfo::GetToken(manualOrderInfo_.Contract);
		params[JSON_SIDE]		= manualOrderInfo_.Side;
		params[JSON_CLIENT]		= manualOrderInfo_.Client;
		params[JSON_ORDER_TYPE] = manualOrderInfo_.Type;
	} else {
		params[JSON_UNIQUE_ID] = manualOrderInfo_.Gateway;
		params[JSON_ORDER_ID]  = manualOrderInfo_.OrderNumber;
	}
	params[JSON_PRICE]	  = FORMAT("{:.2f}", manualOrderInfo_.Price);
	params[JSON_QUANTITY] = manualOrderInfo_.Quantity;

	json[JSON_PARAMS] = params;
	return json.dump();
}

std::string Utils::cancelOrderSerialize(const OrderInfoPtrT& orderInfo_) {
	nlohmann::json json;
	json[JSON_ID] = ++Id;

	nlohmann::json params;
	params[JSON_UNIQUE_ID] = orderInfo_->Gateway;
	params[JSON_ORDER_ID]  = orderInfo_->OrderNo;

	json[JSON_PARAMS] = params;
	return json.dump();
}

std::string Utils::strategySerialize(const StrategyRowPtrT& row_, const std::string& name_, RequestType type_) {
	nlohmann::json json;
	json[JSON_ID] = ++Id;

	nlohmann::json params;
	params[JSON_PF_NUMBER]	   = row_->PF;
	params[JSON_STRATEGY_NAME] = name_;

	if (type_ != RequestType_UNSUBSCRIBE) {
		nlohmann::json arguments;
		for (const auto& [key_, value] : row_->ParameterInfoList) {
			switch (value.Type) {
				case DataType_INT: {
					arguments[key_] = fmt::format("{}", value.Parameter.Integer);
					break;
				}
				case DataType_COMBO:
				case DataType_CLIENT:
				case DataType_TEXT: {
					arguments[key_] = value.Parameter.Text;
					break;
				}
				case DataType_CONTRACT: {
					arguments[key_] = FORMAT("{}", Lancelot::ContractInfo::GetToken(value.Parameter.Text));
					break;
				}
				case DataType_FLOAT: {
					arguments[key_] = FORMAT("{}", value.Parameter.Floating);
					break;
				}
				case DataType_RADIO: {
					arguments[key_] = FORMAT("{}", value.Parameter.Check);
					break;
				}
				case DataType_UPDATES:
				case DataType_END: {
					break;
				}
			}
		}
		params[JSON_ARGUMENTS] = arguments;
	}
	json[JSON_PARAMS] = params;
	return json.dump();
}

bool Utils::ToggleMenuItem(std::string_view window_, bool& open_) {
	const auto info = FORMAT("{} {}", (open_ ? ICON_MD_VISIBILITY : ICON_MD_VISIBILITY_OFF), window_);
	if (ImGui::MenuItem(info.data())) {
		open_ = not open_;
	}
	return open_;
}

void Utils::CreateSupportFolder() {
	std::filesystem::create_directory("Save");
	std::filesystem::create_directory("Config");
	std::filesystem::create_directory("Automation");
}

void Utils::StatusBar() {
	float height = ImGui::GetFrameHeight();
#if 0
	if (ImGui::BeginViewportSideBar("TopSecondMenu##SecondaryMenuBar", nullptr, ImGuiDir_Up, height, MenuBarFlags)) {
		if (ImGui::BeginMenuBar()) {
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
#endif

	if (ImGui::BeginViewportSideBar("StatusBar##MainStatusBar", nullptr, ImGuiDir_Down, height, MenuBarFlags)) {
		if (ImGui::BeginMenuBar()) {
			if (not StatusDisplay.empty()) {
				ImGui::Text("%s", StatusDisplay.data());
			}

			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}

void Utils::RemovePortfolio() {
	std::erase_if(GlobalStrategyList, [](const GlobalStrategyListT::value_type& valueType_) {
		// return (valueType_.second->Status == StrategyStatus_INACTIVE or valueType_.second->Status == StrategyStatus_TERMINATE) and valueType_.second->Selected;
		return valueType_.second.expired();
	});
}

std::optional<WeakStrategyRowPtrT> Utils::GetStrategyRow(uint32_t pf_) {
	auto iterator = GlobalStrategyList.find(pf_);
	if (iterator != GlobalStrategyList.end()) {
		return iterator->second;
	}
	return std::nullopt;
}

void Utils::AppendPortfolio(uint32_t pf_, WeakStrategyRowPtrT ptr_) {
	GlobalStrategyList.emplace(pf_, ptr_);
}

void Utils::ResetPortfolio(StrategyStatus status_) {
	auto _ = std::async(std::launch::async, [&] {
		for (const GlobalStrategyListT::value_type& valueType_ : GlobalStrategyList) {
			if (not valueType_.second.expired()) {
				auto ptr		= valueType_.second.lock();
				ptr->Subscribed = false;
				ptr->Status		= status_;
			}
		};
	});
}

void Utils::DrawTradeRow(const OrderInfoPtrT& tradeInfo_, int& first_, int second_) {
	ImVec4 color = BuySellColor(tradeInfo_->Side);
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	FirstCell(BooksColumnIndex_PF, fmt::format("{}", tradeInfo_->PF).data(), first_, second_);
	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			first_ = second_;
		}
	}

	NextCell(BooksColumnIndex_CONTRACT, "%s", tradeInfo_->Contract.data());
	NextCell(BooksColumnIndex_PRICE, "%.2f", tradeInfo_->Price);
	NextCell(BooksColumnIndex_QUANTITY, "%d", tradeInfo_->Quantity);
	NextCell(BooksColumnIndex_FILLPRICE, "%.2f", tradeInfo_->FillPrice);
	NextCell(BooksColumnIndex_FILLQUANTITY, "%d", tradeInfo_->FillQuantity);
	NextCell(BooksColumnIndex_REMAINING_QTY, "%d", tradeInfo_->Remaining);
	NextCell(BooksColumnIndex_CLIENT, "%s", tradeInfo_->Client.data());
	NextCell(BooksColumnIndex_STATUS, "%s", OrderStatusInfoName[tradeInfo_->StatusValue]);
	NextCell(BooksColumnIndex_TIME, "%s", tradeInfo_->Time.data());
	NextCell(BooksColumnIndex_GATEWAY, "%d", tradeInfo_->Gateway);
	NextCell(BooksColumnIndex_ORDERNUMBER, "%ld", tradeInfo_->OrderNo);
	NextCell(BooksColumnIndex_MESSAGE, "%s", tradeInfo_->Message.data());
	ImGui::PopStyleColor();
}

void		Utils::ContractFilter(ImGuiTextFilter& filter_, std::string& index_) {
#pragma omp parallel
#pragma omp for
	if (ImGui::BeginListBox("##Filter Contract")) {
		for (const auto& contractName : AllContract) {
			if (std::toupper(filter_.InputBuf[0]) == contractName[0] and filter_.PassFilter(contractName.data())) {
				if (ImGui::Selectable(contractName.data())) {
					index_ = contractName;
					// filter_.Clear();
					std::memcpy(filter_.InputBuf, contractName.data(), contractName.length());
				}
			}
		}
		ImGui::EndListBox();
	}
}

void Utils::GetClientList(int userId_) {
	auto table = Lancelot::ContractInfo::GetResultWithIndex(FORMAT(GetClientCode_, userId_));
	ClientCodeList.clear();

	for (const auto& item : table) {
		ClientInfoT clientInfo{.Exchange = Lancelot::ContractInfo::GetExchangeCode(item[ClientIndex_EXCHANGE]), .ClientCode = item[ClientIndex_CLIENTCODE]};
		ClientCodeList.push_back(clientInfo);
		LOG(INFO, "Client Code for User [{}] is [{} {}]", userId_, clientInfo.Exchange, clientInfo.ClientCode);
	}
}

void CreateMarketObject(uint32_t token_, std::string_view name_, float ltp_, float low_, float high_) {
	auto marketData	  = std::make_shared<MarketWatchDataT>();
	marketData->Token = token_;

	std::memset(marketData->Description.data(), '\0', STRATEGY_NAME_LENGTH);
	std::memcpy(marketData->Description.data(), name_.data(), name_.length());
	marketData->LastTradePrice = ltp_;
	marketData->LowPrice	   = low_;
	marketData->HighPrice	   = high_;
	marketData->ClosePrice	   = ltp_;
	MarketWatchDatContainer.emplace(token_, marketData);
}

void Utils::GetAllContractCallback(const Lancelot::ResultSetPtrT result_, float ltp_, float low_, float high_) {
	CreateMarketObject(result_->Token, result_->Description, ltp_, low_, high_);
	AllContract.push_back(result_->Description);
}

double Utils::ScannerAPI(double pf_, double name_, double params_, double token_) {
	BaseScanner::UpdateUser(pf_, FORMAT("Token1={}#Token2={}#Token3={}#Lot={}", token_, token_, token_, token_));
	return 0;
}
