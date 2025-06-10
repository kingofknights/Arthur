//
// Created by VIKLOD on 07-03-2023.
//

#include "Utils.hpp"

#include "API/BaseScanner.hpp"
#include "API/Common.hpp"
#include "API/ContractInfo.hpp"
#include "Configuration.hpp"
#include "Enums.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <future>

extern std::string              StatusDisplay;
extern AllContractT             AllContract;
extern ClientCodeListT          ClientCodeList;
extern MarketWatchDatContainerT MarketWatchDatContainer;
extern SpotInfoT                BankNifty;
extern SpotInfoT                Nifty;
extern SpotInfoT                VIX;

namespace {
    int MessageId = 0;
}

GlobalStrategyListT Utils::GlobalStrategyList;

auto Utils::FormatTimeToString(uint64_t time_) -> std::string {
    char         timestamp[50] = "";
    time_t       secs          = (time_ / 10000000000) + 315513000;
    tm*          ptm           = localtime(&secs);
    size_t       len           = strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", ptm);
    unsigned int ms            = time_ % 1000000000;
    sprintf(timestamp + len, ".%09u", ms);
    return timestamp;
}

auto Utils::ManualSerialize(const OrderFormInfoT& manualOrderInfo_) -> std::string {
    nlohmann::json json;
    json[JSON_ID] = ++MessageId;

    nlohmann::json params;
    if (manualOrderInfo_._orderNumber == 0) {
        params[JSON_TOKEN]      = Lancelot::ContractInfo::GetToken(manualOrderInfo_._contract);
        params[JSON_SIDE]       = manualOrderInfo_._side;
        params[JSON_CLIENT]     = manualOrderInfo_._client;
        params[JSON_ORDER_TYPE] = manualOrderInfo_._type;
    } else {
        params[JSON_UNIQUE_ID] = manualOrderInfo_._uniqueId;
        params[JSON_ORDER_ID]  = manualOrderInfo_._orderNumber;
    }
    params[JSON_PRICE]    = FORMAT("{:.2f}", manualOrderInfo_._price);
    params[JSON_QUANTITY] = manualOrderInfo_._quantity;

    json[JSON_PARAMS] = params;
    return json.dump();
}

auto Utils::CancelOrderSerialize(const OrderInfoPtrT& orderInfo_) -> std::string {
    nlohmann::json json;
    json[JSON_ID] = ++MessageId;

    nlohmann::json params;
    params[JSON_UNIQUE_ID] = orderInfo_->_uniqueId;
    params[JSON_ORDER_ID]  = orderInfo_->_orderNumber;

    json[JSON_PARAMS] = params;
    return json.dump();
}

auto Utils::StrategySerialize(const StrategyRowPtrT& row_, const std::string& name_, Lancelot::RequestType type_) -> std::string {
    nlohmann::json json;
    json[JSON_ID] = ++MessageId;

    nlohmann::json params;
    params[JSON_PF_NUMBER]     = row_->_portfolio;
    params[JSON_STRATEGY_NAME] = name_;

    if (type_ != Lancelot::RequestType_UNSUBSCRIBE) {
        nlohmann::json arguments;
        for (const auto& [key_, value] : row_->_parameterInfoList) {
            switch (value._type) {
                case DataType_INT: {
                    arguments[key_] = FORMAT("{}", value._parameter._integer);
                    break;
                }
                case DataType_COMBO:
                case DataType_CLIENT:
                case DataType_TEXT: {
                    arguments[key_] = value._parameter._text;
                    break;
                }
                case DataType_CONTRACT: {
                    arguments[key_] = FORMAT("{}", Lancelot::ContractInfo::GetToken(value._parameter._text));
                    break;
                }
                case DataType_FLOAT: {
                    arguments[key_] = FORMAT("{}", value._parameter._floating);
                    break;
                }
                case DataType_RADIO: {
                    arguments[key_] = FORMAT("{}", value._parameter._check);
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

auto Utils::ToggleMenuItem(std::string_view window_, bool& open_) -> bool {
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
#if 0
    float height = ImGui::GetFrameHeight();
    if (ImGui::BeginViewportSideBar("TopSecondMenu##SecondaryMenuBar", nullptr, ImGuiDir_Up, height, MenuBarFlags)) {
        if (ImGui::BeginMenuBar()) {
            ImGui::EndMenuBar();
        }
    }
    ImGui::End();

    if (ImGui::BeginViewportSideBar("StatusBar##MainStatusBar", nullptr, ImGuiDir_Down, height, MenuBarFlags)) {
        if (ImGui::BeginMenuBar()) {
            ImGui::EndMenuBar();
        }
    }
    ImGui::End();
#endif
}

void Utils::RemovePortfolio() {
    std::erase_if(GlobalStrategyList, [](const GlobalStrategyListT::value_type& valueType_) {
        // return (valueType_.second->Status == StrategyStatus_INACTIVE or valueType_.second->Status == StrategyStatus_TERMINATE) and valueType_.second->Selected;
        return valueType_.second.expired();
    });
}

auto Utils::GetStrategyRow(uint32_t pf_) -> std::optional<WeakStrategyRowPtrT> {
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
                auto ptr         = valueType_.second.lock();
                ptr->_subscribed = false;
                ptr->_status     = status_;
            }
        };
    });
}

void Utils::DrawTradeRow(const OrderInfoPtrT& tradeInfo_, int& first_, int second_) {
    ImVec4 color = BuySellColor(tradeInfo_->_side);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    FirstCell(BooksColumnIndex_PF, FORMAT("{}", tradeInfo_->_portfolio).data(), first_, second_);
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            first_ = second_;
        }
    }

    NextCell(BooksColumnIndex_CONTRACT, tradeInfo_->_contract.data());
    NextCell(BooksColumnIndex_PRICE, tradeInfo_->_price);
    NextCell(BooksColumnIndex_QUANTITY, tradeInfo_->_quantity);
    NextCell(BooksColumnIndex_FILL_PRICE, tradeInfo_->_fillPrice);
    NextCell(BooksColumnIndex_FILL_QUANTITY, tradeInfo_->_fillQuantity);
    NextCell(BooksColumnIndex_REMAINING_QTY, tradeInfo_->_remaining);
    NextCell(BooksColumnIndex_CLIENT, tradeInfo_->_client.data());
    NextCell(BooksColumnIndex_STATUS, OrderStatusInfoName[tradeInfo_->_statusValue]);
    NextCell(BooksColumnIndex_TIME, tradeInfo_->_time.data());
    NextCell(BooksColumnIndex_GATEWAY, tradeInfo_->_uniqueId);
    NextCell(BooksColumnIndex_ORDER_NUMBER, tradeInfo_->_orderNumber);
    NextCell(BooksColumnIndex_MESSAGE, tradeInfo_->_message.data());
    ImGui::PopStyleColor();
}

void Utils::ContractFilter(ImGuiTextFilter& filter_, std::string& index_) {
#pragma omp parallel
#pragma omp for
    if (ImGui::BeginListBox("##Filter Contract")) {
        for (const auto& contractName : AllContract) {
            if (filter_.PassFilter(contractName.data())) {
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
        ClientInfoT clientInfo{._exchange = Lancelot::ContractInfo::GetExchange(item[ClientIndex_EXCHANGE]), ._clientCode = item[ClientIndex_CLIENT_CODE]};
        ClientCodeList.push_back(clientInfo);
        LOG(INFO, "Client Code for User [{}] is [{} {}]", userId_, Lancelot::ToString(clientInfo._exchange), clientInfo._clientCode);
    }
}

void CreateMarketObject(uint32_t token_, std::string_view name_, float ltp_, float low_, float high_) {
    auto marketData    = std::make_shared<MarketWatchDataT>();
    marketData->_token = token_;

    std::memset(marketData->_description.data(), '\0', StrategyNameLength);
    std::memcpy(marketData->_description.data(), name_.data(), name_.length());
    marketData->_lastTradePrice = ltp_;
    marketData->_low            = low_;
    marketData->_high           = high_;
    marketData->_close          = ltp_;
    MarketWatchDatContainer.emplace(token_, marketData);
}

void Utils::GetAllContractCallback(const Lancelot::ResultSetPtrT result_, float ltp_, float low_, float high_) {
    CreateMarketObject(result_->_token, result_->_description, ltp_, low_, high_);
    AllContract.push_back(result_->_description);
}

double Utils::ScannerAPI(double pf_, double /*name_*/, double params_, double token_) {
    BaseScanner::UpdateUser(pf_, FORMAT("Token1={}#Token2={}#Token3={}#Lot={}", token_, token_, token_, token_));
    return 0;
}

auto Utils::GetPhoenixNewOrder(const OrderFormInfoT& info_, int16_t user_) noexcept -> Lancelot::ManualOrder {
    return Lancelot::ManualOrder{
        ._header = {
            ._type   = Lancelot::RequestType_NEW,
            ._length = sizeof(Lancelot::ManualOrder) - sizeof(Lancelot::Header),
        },
        ._user = {
            ._user      = user_,
            ._portfolio = 0,
        },
        ._token         = static_cast<uint32_t>(info_._marketWatch->_token),
        ._price         = static_cast<uint32_t>(info_._price),
        ._quantity      = static_cast<uint32_t>(info_._quantity),
        ._triggerPrice  = 0,
        ._side          = info_._side,
        ._orderSequence = 0,
        ._orderType     = static_cast<int16_t>(info_._type),
        ._nnf           = 0,
    };
}
auto Utils::GetPhoenixModifyOrder(const OrderFormInfoT& info_, int16_t user_) noexcept -> Lancelot::ModifyOrder {
    return Lancelot::ModifyOrder{
        ._header = {
            ._type   = Lancelot::RequestType_MODIFY,
            ._length = sizeof(Lancelot::ModifyOrder) - sizeof(Lancelot::Header),
        },
        ._user = {
            ._user      = user_,
            ._portfolio = 0,
        },
        ._token         = static_cast<uint32_t>(info_._marketWatch->_token),
        ._orderSequence = static_cast<int32_t>(info_._uniqueId),
        ._price         = static_cast<uint32_t>(info_._price),
        ._quantity      = static_cast<uint32_t>(info_._quantity),
        ._triggerPrice  = 0,
    };
}
auto Utils::GetPhoenixCancelOrder(const OrderInfoPtrT& info_, int16_t user_) noexcept -> Lancelot::CancelOrder {
    return Lancelot::CancelOrder{
        ._header = {
            ._type   = Lancelot::RequestType_CANCEL,
            ._length = sizeof(Lancelot::CancelOrder) - sizeof(Lancelot::Header),
        },
        ._user = {
            ._user      = user_,
            ._portfolio = 0,
        },
        ._token         = info_->_token,
        ._orderSequence = static_cast<int16_t>(info_->_uniqueId),
    };
}
