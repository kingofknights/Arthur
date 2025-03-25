#include "../include/OptionChain.hpp"

#include "../API/ContractInfo.hpp"
#include "../include/Colors.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/Signal.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <Greeks/Greeks.hpp>
#include <ctime>

extern AddContractToDemoSignalT AddContractToDemoSignal;

namespace {
auto FormatTimeToString(time_t rawtime_, std::string format_) -> std::string {
    char   timestamp[50];
    time_t secs = (rawtime_) + 315513000;
    tm*    ptm  = localtime(&secs);
    size_t len  = strftime(timestamp, 20, format_.data(), ptm);
    return { timestamp, len };
}
}// namespace

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
        ImGui::TextColored(UpDownColor(_future->_color._ltp), "LTP : %.2f", _future->_lastTradePrice);
        ImGui::NextColumn();
        ImGui::TextColored(UpDownColor(_future->_pchange), "Change : %.2f", _future->_pchange);
        ImGui::NextColumn();
        ImGui::Text("Bid Qty : %ud", _future->_bid[0]._quantity);
        ImGui::NextColumn();
        ImGui::TextColored(UpDownColor(_future->_color._topBid), "Bid Price : %.2f", _future->_bid[0]._price);
        ImGui::NextColumn();
        ImGui::TextColored(UpDownColor(_future->_color._topAsk), "Ask Price : %.2f", _future->_ask[0]._price);
        ImGui::NextColumn();
        ImGui::Text("Ask Qty : %ud", _future->_ask[0]._quantity);
        ImGui::EndColumns();

        if (ImGui::BeginTable("Option Chain Table", OptionChainColumnIndex_END, TableFlags)) {
            for (const auto& name : OptionChainTableColumnName) {
                ImGui::TableSetupColumn(name, TableColumnFlags | ImGuiTableColumnFlags_NoReorder);
            }
            ImGui::TableHeadersRow();

            for (const OptionChainContainerT::value_type& valueType_ : _optionChainContainer) {
                ImGui::TableNextRow();

                const OptionChainRowT&  optionChainRow = valueType_.second;
                const OptionChainItemT& Call           = optionChainRow._call;
                const OptionChainItemT& Put            = optionChainRow._put;

                float priceForCall = _future->_bid[0]._price ? _future->_bid[0]._price : _future->_lastTradePrice;
                // FIXME : remove abs when working with live contracts
                float expiryGap  = std::abs(Greeks::GetExpiryGap(Put._contract->_expiryDate));
                float rate       = 0.0f;
                float call_IV    = Greeks::GetIV(priceForCall, valueType_.first, rate, expiryGap, Call._marketWatch->_lastTradePrice, true);
                float call_Theta = Greeks::GetTheta(priceForCall, valueType_.first, call_IV, rate, expiryGap, true);
                float call_Vega  = Greeks::GetVega(priceForCall, valueType_.first, call_IV, rate, expiryGap, true);
                float call_Gamma = Greeks::GetGamma(priceForCall, valueType_.first, call_IV, rate, expiryGap, true);
                float call_Delta = Greeks::GetDelta(priceForCall, valueType_.first, call_IV, rate, expiryGap, true);

                float priceForPut = _future->_ask[0]._price ? _future->_ask[0]._price : _future->_lastTradePrice;
                float put_IV      = Greeks::GetIV(priceForPut, valueType_.first, rate, expiryGap, Put._marketWatch->_lastTradePrice, false);
                float put_Theta   = Greeks::GetTheta(priceForPut, valueType_.first, put_IV, rate, expiryGap, false);
                float put_Vega    = Greeks::GetVega(priceForPut, valueType_.first, put_IV, rate, expiryGap, false);
                float put_Gamma   = Greeks::GetGamma(priceForPut, valueType_.first, put_IV, rate, expiryGap, false);
                float put_Delta   = Greeks::GetDelta(priceForPut, valueType_.first, put_IV, rate, expiryGap, false);

                ImU32 color     = ImGui::GetColorU32(COLOR_GRAY);
                bool  needColor = valueType_.first < _future->_lastTradePrice;

                NextCell(OptionChainColumnIndex_CALL_OI, "%d", Call._marketWatch->_openInterest);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_VOLUME, "%d", Call._marketWatch->_volumeTradedToday);
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

                NextCell(OptionChainColumnIndex_CALL_IV, "%.2f", call_IV * 100.0F);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_LTP, "%.2f", Call._marketWatch->_lastTradePrice, UpDownColor(Call._marketWatch->_color._ltp));
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_CHANGE, "%.2f", Call._marketWatch->_pchange);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_BID_QTY, "%d", Call._marketWatch->_bid[0]._quantity);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_BID_PRICE, "%.2f", Call._marketWatch->_bid[0]._price, UpDownColor(Call._marketWatch->_color._topBid));
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_ASK_PRICE, "%.2f", Call._marketWatch->_ask[0]._price, UpDownColor(Call._marketWatch->_color._topAsk));
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_ASK_QTY, "%d", Call._marketWatch->_ask[0]._quantity);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }

                NextCell(OptionChainColumnIndex_STRIKE_PRICE, "%.2f", valueType_.first);

                NextCell(OptionChainColumnIndex_PUT_BID_QTY, "%d", Put._marketWatch->_bid[0]._quantity);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_BID_PRICE, "%.2f", Put._marketWatch->_bid[0]._price, UpDownColor(Put._marketWatch->_color._topBid));
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_ASK_PRICE, "%.2f", Put._marketWatch->_ask[0]._price, UpDownColor(Put._marketWatch->_color._topAsk));
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_ASK_QTY, "%d", Put._marketWatch->_ask[0]._quantity);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_CHANGE, "%.2f", Put._marketWatch->_pchange);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_LTP, "%.2f", Put._marketWatch->_lastTradePrice, UpDownColor(Put._marketWatch->_color._ltp));
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

                NextCell(OptionChainColumnIndex_PUT_VOLUME, "%ld", Put._marketWatch->_volumeTradedToday);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_OI, "%d", Put._marketWatch->_openInterest);
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
    int      token_  = Lancelot::ContractInfo::GetToken(contract_);
    uint32_t expiry_ = Lancelot::ContractInfo::GetExpiryDate(token_);

    if (not Lancelot::ContractInfo::IsFuture(token_)) {
        token_ = Lancelot::ContractInfo::GetFuture(token_);
    }
    auto future = ContractInfo::GetLiveDataRef(token_);

    if (!future) return;
    _optionChainContainer.clear();

    _future = future;

    AddContractToDemoSignal(future->_token);
    uint32_t futurePrice_ = future->_lastTradePrice * 100.0;
    _symbol               = Lancelot::ContractInfo::GetSymbol(token_);
    _expiry               = FormatTimeToString(expiry_, "%d-%m-%Y");

    LoadOptions(_symbol, expiry_, futurePrice_, '>', "asc");
    LoadOptions(_symbol, expiry_, futurePrice_, '<', "desc");
}
void OptionChain::LoadOptions(const std::string& symbol_, uint32_t expiry_, uint32_t futurePrice_, char comparator_, const std::string& order_) {
    auto query = FORMAT(GetOptionChain_, symbol_, symbol_, expiry_, comparator_, futurePrice_, order_);
    auto table = Lancelot::ContractInfo::GetResultWithIndex(query);

    for (const auto& row : table) {
        auto             callToken = std::stoi(row[0]);
        auto             putToken  = std::stoi(row[1]);
        OptionChainItemT call{ ._marketWatch = ContractInfo::GetLiveDataRef(callToken), ._contract = Lancelot::ContractInfo::GetResultSet(callToken) };
        OptionChainItemT put{ ._marketWatch = ContractInfo::GetLiveDataRef(putToken), ._contract = Lancelot::ContractInfo::GetResultSet(putToken) };
        OptionChainRowT  optionChainRow{ ._call = call, ._put = put };

        _optionChainContainer.emplace(std::stof(row[2]) / call._contract->_divisor, optionChainRow);
        AddContractToDemoSignal(callToken);
        AddContractToDemoSignal(putToken);
    }
}
