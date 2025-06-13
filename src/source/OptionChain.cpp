#include "OptionChain.hpp"

#include "API/ContractInfo.hpp"
#include "Colors.hpp"
#include "Configuration.hpp"
#include "Enums.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#include <Greeks/Greeks.hpp>

#include <ctime>

namespace {
    auto FormatTimeToString(time_t rawtime_, std::string format_) -> std::string {
        char   timestamp[50];
        time_t secs = (rawtime_) + 315513000;
        tm*    ptm  = localtime(&secs);
        size_t len  = strftime(timestamp, 20, format_.data(), ptm);
        return {timestamp, len};
    }
}  // namespace

OptionChain::OptionChain() : _future(std::make_shared<MarketWatchDataT>()) {}

void OptionChain::Paint(bool* show_) {
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
        ImGui::TextColored(UpDownColor(_future->_pchange > 0), "Change : %.2f", _future->_pchange);
        ImGui::NextColumn();
        ImGui::Text("Bid Qty : %u", _future->_bid[0]._quantity);
        ImGui::NextColumn();
        ImGui::TextColored(UpDownColor(_future->_color._topBid), "Bid Price : %.2f", _future->_bid[0]._price);
        ImGui::NextColumn();
        ImGui::TextColored(UpDownColor(_future->_color._topAsk), "Ask Price : %.2f", _future->_ask[0]._price);
        ImGui::NextColumn();
        ImGui::Text("Ask Qty : %u", _future->_ask[0]._quantity);
        ImGui::EndColumns();

        if (ImGui::BeginTable("Option Chain Table", OptionChainColumnIndex_END, TableFlags)) {
            for (const auto& name : OptionChainTableColumnName) {
                ImGui::TableSetupColumn(name, TableColumnFlags | ImGuiTableColumnFlags_NoReorder);
            }
            ImGui::TableHeadersRow();

            for (const auto& valueType : _optionChainContainer) {
                ImGui::TableNextRow();

                const OptionChainRowT&  optionChainRow = valueType.second;
                const OptionChainItemT& call           = optionChainRow._call;
                const OptionChainItemT& put            = optionChainRow._put;

                float priceForCall = _future->_bid[0]._price > 0 ? _future->_bid[0]._price : _future->_lastTradePrice;
                // FIXME : remove abs when working with live contracts
                double expiryGap  = std::abs(Greeks::GetExpiryGap(put._contract->_expiryDate));
                double rate       = 0.0;
                double call_IV    = Greeks::GetIV(priceForCall, valueType.first, rate, expiryGap, call._marketWatch->_lastTradePrice, true);
                double call_Theta = Greeks::GetTheta(priceForCall, valueType.first, call_IV, rate, expiryGap, true);
                double call_Vega  = Greeks::GetVega(priceForCall, valueType.first, call_IV, rate, expiryGap, true);
                double call_Gamma = Greeks::GetGamma(priceForCall, valueType.first, call_IV, rate, expiryGap, true);
                double call_Delta = Greeks::GetDelta(priceForCall, valueType.first, call_IV, rate, expiryGap, true);

                float  priceForPut = _future->_ask[0]._price > 0 ? _future->_ask[0]._price : _future->_lastTradePrice;
                double put_IV      = Greeks::GetIV(priceForPut, valueType.first, rate, expiryGap, put._marketWatch->_lastTradePrice, false);
                double put_Theta   = Greeks::GetTheta(priceForPut, valueType.first, put_IV, rate, expiryGap, false);
                double put_Vega    = Greeks::GetVega(priceForPut, valueType.first, put_IV, rate, expiryGap, false);
                double put_Gamma   = Greeks::GetGamma(priceForPut, valueType.first, put_IV, rate, expiryGap, false);
                double put_Delta   = Greeks::GetDelta(priceForPut, valueType.first, put_IV, rate, expiryGap, false);

                ImU32 color     = ImGui::GetColorU32(COLOR_GRAY);
                bool  needColor = valueType.first < _future->_lastTradePrice;

                NextCell(OptionChainColumnIndex_CALL_OI, call._marketWatch->_openInterest);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_VOLUME, call._marketWatch->_volumeTradedToday);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_DELTA, call_Delta);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_GAMMA, call_Gamma);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_VEGA, call_Vega);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_THETA, call_Theta);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }

                NextCell(OptionChainColumnIndex_CALL_IV, (call_IV * 100.0));
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_LTP, call._marketWatch->_lastTradePrice, UpDownColor(call._marketWatch->_color._ltp));
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_CHANGE, call._marketWatch->_pchange);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_BID_QTY, call._marketWatch->_bid[0]._quantity);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_BID_PRICE, call._marketWatch->_bid[0]._price, UpDownColor(call._marketWatch->_color._topBid));
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_ASK_PRICE, call._marketWatch->_ask[0]._price, UpDownColor(call._marketWatch->_color._topAsk));
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_CALL_ASK_QTY, call._marketWatch->_ask[0]._quantity);
                if (needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }

                NextCell(OptionChainColumnIndex_STRIKE_PRICE, float(valueType.first));
                // NextCell(OptionChainColumnIndex_STRIKE_PRICE, static_cast<int>(valueType.first * 100.0));

                NextCell(OptionChainColumnIndex_PUT_BID_QTY, put._marketWatch->_bid[0]._quantity);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_BID_PRICE, put._marketWatch->_bid[0]._price, UpDownColor(put._marketWatch->_color._topBid));
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_ASK_PRICE, put._marketWatch->_ask[0]._price, UpDownColor(put._marketWatch->_color._topAsk));
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_ASK_QTY, put._marketWatch->_ask[0]._quantity);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_CHANGE, put._marketWatch->_pchange);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_LTP, put._marketWatch->_lastTradePrice, UpDownColor(put._marketWatch->_color._ltp));
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_IV, (put_IV * 100.0));
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }

                NextCell(OptionChainColumnIndex_PUT_THETA, put_Theta);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }

                NextCell(OptionChainColumnIndex_PUT_VEGA, put_Vega);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }

                NextCell(OptionChainColumnIndex_PUT_GAMMA, put_Gamma);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_DELTA, put_Delta);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }

                NextCell(OptionChainColumnIndex_PUT_VOLUME, put._marketWatch->_volumeTradedToday);
                if (not needColor) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, color);
                }
                NextCell(OptionChainColumnIndex_PUT_OI, put._marketWatch->_openInterest);
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
        OptionChainItemT call{._marketWatch = ContractInfo::GetLiveDataRef(callToken), ._contract = Lancelot::ContractInfo::GetResultSet(callToken)};
        OptionChainItemT put{._marketWatch = ContractInfo::GetLiveDataRef(putToken), ._contract = Lancelot::ContractInfo::GetResultSet(putToken)};
        OptionChainRowT  optionChainRow{._call = call, ._put = put};

        _optionChainContainer.emplace(std::stof(row[2]) / call._contract->_divisor, optionChainRow);
    }
}
