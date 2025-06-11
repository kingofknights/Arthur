#include "Position.hpp"

#include "API/Common.hpp"
#include "API/ContractInfo.hpp"
#include "Configuration.hpp"
#include "IconsMaterialDesign.h"
#include "Logger.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"
#include "imgui.h"
#include "plf_nanotimer.h"

#include <Greeks/Greeks.hpp>
#include <ImGuiFileDialog.h>

#include <iterator>
#include <numeric>
#include <sstream>

template <typename Type>
void UpdateTradeInfoNetbook(Type& data, const OrderInfoPtrT& tradeInfo_) {
    if (tradeInfo_->_side == Lancelot::Side_BUY) {
        data->_totalBuyPrice += tradeInfo_->_price * tradeInfo_->_quantity;
        data->_buyQuantity += tradeInfo_->_quantity;
        data->_averageBuyPrice = data->_totalBuyPrice / data->_buyQuantity;
    } else {
        data->_totalSellPrice += tradeInfo_->_price * tradeInfo_->_quantity;
        data->_sellQuantity += tradeInfo_->_quantity;
        data->_averageSellPrice = data->_totalSellPrice / data->_sellQuantity;
    }
}

template <typename Container, typename Sequencial, typename Key>
void UpdateNetBook(Container& container_, Sequencial& sequencial_, Key key_, const OrderInfoPtrT& tradeInfo_) {
    auto iterator = container_.find(key_);
    if (iterator != container_.end()) {
        NetBookColumnPtrT& data = iterator->second;
        UpdateTradeInfoNetbook(data, tradeInfo_);
        data->_netInvestment = data->_totalBuyPrice - data->_totalSellPrice;
        data->_totalQuantity = data->_buyQuantity - data->_sellQuantity;
    } else {
        NetBookColumnPtrT data  = std::make_shared<NetBookColumnT>();
        data->_averageBuyPrice  = 0;
        data->_averageSellPrice = 0;
        data->_buyQuantity      = 0;
        data->_sellQuantity     = 0;
        data->_totalBuyPrice    = 0;
        data->_totalSellPrice   = 0;
        data->_lastTradePrice   = 0;
        data->_lastPNL          = 0;
        data->_portfolio        = tradeInfo_->_portfolio;
        data->_marketWatch      = ContractInfo::GetLiveDataRef(tradeInfo_->_token);
        UpdateTradeInfoNetbook(data, tradeInfo_);
        data->_netInvestment = data->_totalBuyPrice - data->_totalSellPrice;
        data->_totalQuantity = data->_buyQuantity - data->_sellQuantity;

        container_.emplace(key_, data);
        sequencial_.push_back(std::make_pair(key_, data));
        std::sort(sequencial_.begin(), sequencial_.end());
    }
}

void Position::DrawBook(bool* show_) {
    if (ImGui::Begin("NetBooks", show_, ImGuiWindowFlags_NoTitleBar)) {
        if (ImGui::BeginTabBar("Net Book Canvas", StrategyWorkspaceTabFlags)) {
            if (ImGui::BeginTabItem("SymbolWise Netbook")) {
                _calculation = NetBookCalculation_SYMBOL;
                DrawSymbolWiseNetBook();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("PFWise Netbook")) {
                _calculation = NetBookCalculation_PF;
                DrawPFWiseNetBook();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Greek book")) {
                _calculation = NetBookCalculation_GREEK;
                DrawGreekNetBook();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}

Position::Position(boost::asio::io_context& ioContext_) : _timer(ioContext_) {
    TimerEvent();
}

void Position::Insert(const OrderInfoPtrT& tradeInfo_) {
    // SymbolBoolWiseBookUpdate(tradeInfo_);
    // PFWiseBookUpdate(tradeInfo_);
    // GreekBookUpdate(tradeInfo_);
    _pendingTradeUpdate.push(tradeInfo_);
}

void Position::Paint(bool* show_) {
    _pendingTradeUpdate.consume_all([this](OrderInfoPtrT tradeInfo_) {
        SymbolBoolWiseBookUpdate(tradeInfo_);
        PFWiseBookUpdate(tradeInfo_);
        GreekBookUpdate(tradeInfo_);
    });

    if (*show_) {
        DrawBook(show_);
    }
}

void Position::SymbolBoolWiseBookUpdate(const OrderInfoPtrT& tradeInfo_) {
    UpdateNetBook(_symbolWiseTradeContainer, _symbolWiseTradeContainerVec, tradeInfo_->_token, tradeInfo_);
}

void Position::PFWiseBookUpdate(const OrderInfoPtrT& tradeInfo_) {
    std::pair<int, int> key = std::make_pair(tradeInfo_->_portfolio, tradeInfo_->_token);
    UpdateNetBook(_pFWiseTradeContainer, _pFWiseTradeContainerVec, key, tradeInfo_);
}

void Position::GreekBookUpdate(const OrderInfoPtrT& tradeInfo_) {
    auto iteratorContainer = _greekBookContainer.find(tradeInfo_->_token);
    if (iteratorContainer != _greekBookContainer.end()) {
        GreekBookColumnPtrT& data = iteratorContainer->second;
        UpdateTradeInfoNetbook(data, tradeInfo_);

    } else {
        GreekBookColumnPtrT data = std::make_shared<GreekBookColumnT>();

        GreeksPtrT greek = std::make_shared<GreeksT>();
        {
            auto* resultSet     = Lancelot::ContractInfo::GetResultSet(tradeInfo_->_token);
            greek->_isCall      = resultSet->_option == Lancelot::OptionType_CALL;
            greek->_isFuture    = resultSet->_instType == Lancelot::Instrument_FUTURE;
            greek->_expiry      = resultSet->_expiryDate;
            greek->_strike      = resultSet->_strikePrice;
            greek->_impliedVol  = 1;
            greek->_delta       = 1;
            greek->_gamma       = 1;
            greek->_vega        = 1;
            greek->_theta       = 1;
            greek->_marketWatch = ContractInfo::GetLiveDataRef(tradeInfo_->_token);
            greek->_future      = ContractInfo::GetLiveDataRef(Lancelot::ContractInfo::GetFuture(tradeInfo_->_token));
            _greekList.push_back(greek);
        }

        _greekList.push_back(greek);

        data->_symbol           = Lancelot::ContractInfo::GetSymbol(tradeInfo_->_token);
        data->_buyQuantity      = 0;
        data->_sellQuantity     = 0;
        data->_totalSellPrice   = 0;
        data->_totalBuyPrice    = 0;
        data->_averageSellPrice = 0;
        data->_averageBuyPrice  = 0;
        data->_greeks           = greek;

        UpdateTradeInfoNetbook(data, tradeInfo_);
        _greekBookContainer.emplace(tradeInfo_->_token, data);
    }
}

double Position::CalculateSymbolWisePNL(const NetBookColumnPtrT& column) {
    if (column->_lastTradePrice == column->_marketWatch->_lastTradePrice) {
        return column->_lastPNL;
    }
    column->_lastTradePrice = column->_marketWatch->_lastTradePrice;

    float mtm = 0;
    float bpl = std::min(column->_buyQuantity, column->_sellQuantity) * (column->_averageSellPrice - column->_averageBuyPrice);
    if (column->_buyQuantity > column->_sellQuantity) {
        mtm = (column->_buyQuantity - column->_sellQuantity) * (column->_lastTradePrice - column->_averageBuyPrice);
    } else if (column->_buyQuantity < column->_sellQuantity) {
        mtm = (column->_sellQuantity - column->_buyQuantity) * (column->_averageSellPrice - column->_lastTradePrice);
    } else {
        bpl = column->_totalSellPrice - column->_totalBuyPrice;
    }

    float pnl        = mtm + bpl;
    column->_mtm     = mtm;
    column->_pnl     = pnl;
    column->_lastPNL = pnl;
    return pnl;
}

void Position::DrawSymbolWiseNetBook() {
    const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

    if (ImGui::BeginTable("Symbol Wise NetBookTable", SymbolWiseNetBookColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
        ImGui::TableSetupScrollFreeze(1, 0);
        for (const auto& name : SymbolWiseTableColumnName) {
            ImGui::TableSetupColumn(name, TableColumnFlags);
        }
        ImGui::TableHeadersRow();

        _clipper.Begin(_symbolWiseTradeContainerVec.size());
        while (_clipper.Step()) {
            auto begin = _symbolWiseTradeContainerVec.begin() + _clipper.DisplayStart;
            auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
            for (auto iterator = begin; iterator < end; ++iterator) {
                ImGui::TableNextRow();
                auto& column = iterator->second;
                NextCell(SymbolWiseNetBookColumnIndex_CONTRACT, column->_marketWatch->_description.data());
                NextCell(SymbolWiseNetBookColumnIndex_AVGBID, column->_averageBuyPrice);
                NextCell(SymbolWiseNetBookColumnIndex_BUYQTY, column->_buyQuantity);
                NextCell(SymbolWiseNetBookColumnIndex_SELLQTY, column->_sellQuantity);
                NextCell(SymbolWiseNetBookColumnIndex_AVGSELL, column->_averageSellPrice);
                NextCell(SymbolWiseNetBookColumnIndex_TOTAL, column->_totalQuantity, UpDownColor(column->_totalQuantity));
                NextCell(SymbolWiseNetBookColumnIndex_NETINVEST, column->_netInvestment, UpDownColor(column->_netInvestment));
                NextCell(SymbolWiseNetBookColumnIndex_MTM, column->_mtm, UpDownColor(column->_mtm));
                NextCell(SymbolWiseNetBookColumnIndex_LTP, column->_marketWatch->_lastTradePrice, UpDownColor(column->_marketWatch->_color._ltp));
                NextCell(SymbolWiseNetBookColumnIndex_PNL, column->_pnl, UpDownColor(column->_pnl));
            }
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::TextColored(UpDownColor(_netPNL >= 0), "| Net PNL : %.2f |", _netPNL);
    ImGui::SameLine();
    if (ImGui::Button("Export")) {
        ImGuiFileDialog::Instance()->OpenDialog("Export Trades", "File Manager", ".csv");
    }
    DrawExportSymbolWisePostion();
}

void Position::DrawExportSymbolWisePostion() {
    if (ImGuiFileDialog::Instance()->Display("Export Trades")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string  filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::fstream file(filePathName, std::ios::out);
            if (file.is_open()) {
                for (const auto& row : _symbolWiseTradeContainerVec) {
                    const auto& column = row.second;
                    file << FORMAT("{},{},{},{},{},{},{},{},{},{}", column->_marketWatch->_description.data(), column->_averageBuyPrice, column->_buyQuantity, column->_sellQuantity,
                                   column->_averageSellPrice, column->_totalQuantity, column->_netInvestment, column->_mtm, column->_marketWatch->_lastTradePrice, column->_pnl);
                }
            }
            file.close();
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void Position::DrawPFWiseNetBook() {
    const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

    if (ImGui::BeginTable("PF Wise NetBookTable", PFWiseNetBookColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
        ImGui::TableSetupScrollFreeze(2, 0);
        for (const auto& name : PFWiseTableColumnName) {
            ImGui::TableSetupColumn(name, TableColumnFlags);
        }
        ImGui::TableHeadersRow();

        _clipper.Begin(_pFWiseTradeContainerVec.size());
        while (_clipper.Step()) {
            auto begin = _pFWiseTradeContainerVec.begin() + _clipper.DisplayStart;
            auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
            for (auto iterator = begin; iterator < end; ++iterator) {
                ImGui::TableNextRow();
                auto& column = iterator->second;

                NextCell(PFWiseNetBookColumnIndex_PF, column->_portfolio);
                NextCell(PFWiseNetBookColumnIndex_CONTRACT, column->_marketWatch->_description.data());
                NextCell(PFWiseNetBookColumnIndex_AVGBID, column->_averageBuyPrice);
                NextCell(PFWiseNetBookColumnIndex_BUYQTY, column->_buyQuantity);
                NextCell(PFWiseNetBookColumnIndex_SELLQTY, column->_sellQuantity);
                NextCell(PFWiseNetBookColumnIndex_AVGSELL, column->_averageSellPrice);
                NextCell(PFWiseNetBookColumnIndex_TOTAL, column->_totalQuantity, UpDownColor(column->_totalQuantity));
                NextCell(PFWiseNetBookColumnIndex_NETINVEST, column->_netInvestment, UpDownColor(column->_netInvestment));
                NextCell(PFWiseNetBookColumnIndex_MTM, column->_mtm, UpDownColor(column->_mtm));
                NextCell(PFWiseNetBookColumnIndex_LTP, column->_marketWatch->_lastTradePrice, UpDownColor(column->_marketWatch->_color._ltp));
                NextCell(PFWiseNetBookColumnIndex_PNL, column->_pnl, UpDownColor(column->_pnl));
            }
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::TextColored(UpDownColor(_netPNL), "| Net PNL : %.2f |", _netPNL);
}

void Position::DrawGreekNetBook() {
    float m2M   = 0;
    float theta = 0;
    float vega  = 0;

    std::unordered_map<std::string, DValueT> valueMap;

    for (auto& [key, column] : _greekBookContainer) {
        DValueT DValue;
        float   Qty = (column->_buyQuantity - column->_sellQuantity);
        float   LTP = column->_greeks->_future->_lastTradePrice;
        DValue._ltp = LTP;
        if (column->_buyQuantity > column->_sellQuantity) {
            DValue._mtm = (Qty) * (LTP - column->_averageBuyPrice);
        } else if (column->_buyQuantity < column->_sellQuantity) {
            DValue._mtm = (-Qty) * (column->_averageSellPrice - LTP);
        }

        DValue._delta = Qty * column->_greeks->_delta;
        DValue._gamma = Qty * column->_greeks->_gamma;
        DValue._vega  = Qty * column->_greeks->_vega;
        DValue._theta = Qty * column->_greeks->_theta;

        m2M += DValue._mtm;
        theta += DValue._theta;
        vega += DValue._vega;

        auto iterator = valueMap.find(column->_symbol);
        if (iterator != valueMap.end()) {
            auto& value = iterator->second;
            value._delta += DValue._delta;
            value._gamma += DValue._gamma;
            value._vega += DValue._vega;
            value._theta += DValue._theta;
            value._mtm += DValue._mtm;
            value._value = value._ltp * value._delta;
        } else {
            DValue._symbol = column->_symbol;
            valueMap.emplace(column->_symbol, DValue);
        }
    }

    const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

    if (ImGui::BeginTable("Greek Book Table", GreekBookColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
        ImGui::TableSetupScrollFreeze(0, 0);

        for (const auto& name : GreekBookColumnName) {
            ImGui::TableSetupColumn(name, TableColumnFlags);
        }
        ImGui::TableHeadersRow();

        _clipper.Begin(static_cast<int>(valueMap.size()));
        while (_clipper.Step()) {
            auto begin = valueMap.begin();
            std::ranges::advance(begin, _clipper.DisplayStart);
            auto end = begin;
            std::ranges::advance(end, _clipper.DisplayEnd - _clipper.DisplayStart);
            for (auto iterator = begin; iterator != end; ++iterator) {
                ImGui::TableNextRow();
                const auto& value = iterator->second;
                NextCell(GreekBookColumnIndex_SYMBOL, iterator->first.data());
                NextCell(GreekBookColumnIndex_DELTA, value._delta, UpDownColor(value._delta));
                NextCell(GreekBookColumnIndex_GAMMA, value._gamma, UpDownColor(value._gamma));
                NextCell(GreekBookColumnIndex_VEGA, value._vega, UpDownColor(value._vega));
                NextCell(GreekBookColumnIndex_THETA, value._theta, UpDownColor(value._theta));
                NextCell(GreekBookColumnIndex_MTM, value._mtm, UpDownColor(value._mtm));
                NextCell(GreekBookColumnIndex_MARKETRATE, value._ltp, UpDownColor(value._ltp));
                NextCell(GreekBookColumnIndex_VALUE, value._value, UpDownColor(value._value));
            }
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::TextColored(UpDownColor(m2M), "| MTM : %.4f |", m2M);
    ImGui::SameLine();
    ImGui::TextColored(UpDownColor(theta), "| Theta : %.4f |", theta);
    ImGui::SameLine();
    ImGui::TextColored(UpDownColor(vega), "| Vega : %.4f |", vega);
}

void Position::UpdateGreekValue() {
    if (_calculation == NetBookCalculation_GREEK) {
        for (const GreeksPtrT& column : _greekList) {
            if (column->_isFuture) {
                continue;
            }
            double ExpiryGap = std::abs(Greeks::GetExpiryGap(column->_expiry));
            float  LTP       = column->_isCall ? (column->_future->_bid[0]._price ? column->_future->_bid[0]._price : column->_future->_lastTradePrice)
                                               : (column->_future->_ask[0]._price ? column->_future->_ask[0]._price : column->_future->_lastTradePrice);

            column->_impliedVol = Greeks::GetIV(LTP, column->_strike, 0, ExpiryGap, column->_marketWatch->_lastTradePrice, column->_isCall);
            column->_delta      = Greeks::GetDelta(LTP, column->_strike, column->_impliedVol, 0, ExpiryGap, column->_isCall);
            column->_gamma      = Greeks::GetGamma(LTP, column->_strike, column->_impliedVol, 0, ExpiryGap, column->_isCall);
            column->_vega       = Greeks::GetVega(LTP, column->_strike, column->_impliedVol, 0, ExpiryGap, column->_isCall);
            column->_theta      = Greeks::GetTheta(LTP, column->_strike, column->_impliedVol, 0, ExpiryGap, column->_isCall);
        }
    } else if (_calculation == NetBookCalculation_SYMBOL) {
        auto copy = _symbolWiseTradeContainerVec;
        _netPNL   = std::accumulate(copy.begin(), copy.end(), 0, [&](double sum, const auto& pair_) { return sum + CalculateSymbolWisePNL(pair_.second); });
    } else if (_calculation == NetBookCalculation_PF) {
        auto copy = _pFWiseTradeContainerVec;
        _netPNL   = std::accumulate(copy.begin(), copy.end(), 0, [&](double sum, const auto& pair_) { return sum + CalculateSymbolWisePNL(pair_.second); });
    }

    TimerEvent();
}

void Position::TimerEvent() {
    _timer.expires_from_now(boost::posix_time::seconds(1));
    _timer.async_wait([&](const boost::system::error_code& err_) { if(not err_) {
    UpdateGreekValue(); 
} });
}
