#include "../include/Position.hpp"

#include <Greeks/Greeks.hpp>
#include <iterator>
#include <numeric>

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Configuration.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/plf_nanotimer.h"

template<typename Type>
void UpdateTradeInfoNetbook(Type& data, const OrderInfoPtrT& tradeInfo_) {
    if (tradeInfo_->Side == Lancelot::Side_BUY) {
        data->TotalBuyPrice += tradeInfo_->_price * tradeInfo_->_quantity;
        data->_buyQuantity += tradeInfo_->_quantity;
        data->AverageBuyPrice = data->TotalBuyPrice / data->_buyQuantity;
    } else {
        data->TotalSellPrice += tradeInfo_->_price * tradeInfo_->_quantity;
        data->_sellQuantity += tradeInfo_->_quantity;
        data->AverageSellPrice = data->TotalSellPrice / data->_sellQuantity;
    }
}

template<typename Container, typename Sequencial, typename Key>
void UpdateNetBook(Container& container_, Sequencial& sequencial_, Key key_, const OrderInfoPtrT& tradeInfo_) {
    auto iterator = container_.find(key_);
    if (iterator != container_.end()) {
        NetBookColumnPtrT& data = iterator->second;
        UpdateTradeInfoNetbook(data, tradeInfo_);
        data->NetInvestment = data->TotalBuyPrice - data->TotalSellPrice;
        data->TotalQty      = data->_buyQuantity - data->_sellQuantity;
    } else {
        NetBookColumnPtrT data = std::make_shared<NetBookColumnT>();
        data->AverageBuyPrice  = 0;
        data->AverageSellPrice = 0;
        data->_buyQuantity     = 0;
        data->_sellQuantity    = 0;
        data->TotalBuyPrice    = 0;
        data->TotalSellPrice   = 0;
        data->LastLTP          = 0;
        data->LastPNL          = 0;
        data->PF               = tradeInfo_->_portfolio;
        data->Self             = ContractInfo::GetLiveDataRef(tradeInfo_->_token);
        UpdateTradeInfoNetbook(data, tradeInfo_);
        data->NetInvestment = data->TotalBuyPrice - data->TotalSellPrice;
        data->TotalQty      = data->_buyQuantity - data->_sellQuantity;

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

void Position::paint(bool* show_) {
    _pendingTradeUpdate.consume_one([this](const OrderInfoPtrT& tradeInfo_) {
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
            auto* resultSet    = Lancelot::ContractInfo::GetResultSet(tradeInfo_->_token);
            greek->_isCall     = resultSet->_option == Lancelot::OptionType_CALL;
            greek->_isFuture   = resultSet->_instType == Lancelot::Instrument_FUTURE;
            greek->_expiry     = resultSet->_expiryDate;
            greek->_strike     = resultSet->_strikePrice;
            greek->_impliedVol = 1;
            greek->_delta      = 1;
            greek->_gamma      = 1;
            greek->_vega       = 1;
            greek->_theta      = 1;
            greek->_self       = ContractInfo::GetLiveDataRef(tradeInfo_->_token);
            greek->_future     = ContractInfo::GetLiveDataRef(Lancelot::ContractInfo::GetFuture(tradeInfo_->_token));
            _greekList.push_back(greek);
        }

        _greekList.push_back(greek);

        data->Symbol           = Lancelot::ContractInfo::GetSymbol(tradeInfo_->_token);
        data->_buyQuantity     = 0;
        data->_sellQuantity    = 0;
        data->TotalSellPrice   = 0;
        data->TotalBuyPrice    = 0;
        data->AverageSellPrice = 0;
        data->AverageBuyPrice  = 0;
        data->Greeks           = greek;

        UpdateTradeInfoNetbook(data, tradeInfo_);
        _greekBookContainer.emplace(tradeInfo_->_token, data);
    }
}

double Position::CalculateSymbolWisePNL(const NetBookColumnPtrT& column) {
    if (column->LastLTP == column->Self->_ltp) {
        return column->LastPNL;
    }
    column->LastLTP = column->Self->_ltp;

    double MTM = 0;
    float  BPL = std::min(column->_buyQuantity, column->_sellQuantity) * (column->AverageSellPrice - column->AverageBuyPrice);
    if (column->_buyQuantity > column->_sellQuantity) {
        MTM = (column->_buyQuantity - column->_sellQuantity) * (column->LastLTP - column->AverageBuyPrice);
    } else if (column->_buyQuantity < column->_sellQuantity) {
        MTM = (column->_sellQuantity - column->_buyQuantity) * (column->AverageSellPrice - column->LastLTP);
    } else {
        BPL = column->TotalSellPrice - column->TotalBuyPrice;
    }

    double PNL      = MTM + BPL;
    column->MTM     = MTM;
    column->PNL     = PNL;
    column->LastPNL = PNL;
    return PNL;
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
                auto column = iterator->second;
                NextCell(SymbolWiseNetBookColumnIndex_CONTRACT, "%s", column->Self->_description.data());
                NextCell(SymbolWiseNetBookColumnIndex_AVGBID, "%0.2f", column->AverageBuyPrice);
                NextCell(SymbolWiseNetBookColumnIndex_BUYQTY, "%d", column->_buyQuantity);
                NextCell(SymbolWiseNetBookColumnIndex_SELLQTY, "%d", column->_sellQuantity);
                NextCell(SymbolWiseNetBookColumnIndex_AVGSELL, "%0.2f", column->AverageSellPrice);
                NextCell(SymbolWiseNetBookColumnIndex_TOTAL, "%d", column->TotalQty, UpDownColor(column->TotalQty));
                NextCell(SymbolWiseNetBookColumnIndex_NETINVEST, "%.2f", column->NetInvestment, UpDownColor(column->NetInvestment));
                NextCell(SymbolWiseNetBookColumnIndex_MTM, "%d", column->MTM, UpDownColor(column->MTM));
                NextCell(SymbolWiseNetBookColumnIndex_LTP, "%.2f", column->Self->_ltp, UpDownColor(column->Self->_color._ltp));
                NextCell(SymbolWiseNetBookColumnIndex_PNL, "%d", column->PNL, UpDownColor(column->PNL));
            }
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::TextColored(UpDownColor(_netPNL), "| Net PNL : %.2f |", _netPNL);
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
                auto column = iterator->second;

                NextCell(PFWiseNetBookColumnIndex_PF, "%d", column->PF);
                NextCell(PFWiseNetBookColumnIndex_CONTRACT, "%s", column->Self->_description.data());
                NextCell(PFWiseNetBookColumnIndex_AVGBID, "%0.2f", column->AverageBuyPrice);
                NextCell(PFWiseNetBookColumnIndex_BUYQTY, "%d", column->_buyQuantity);
                NextCell(PFWiseNetBookColumnIndex_SELLQTY, "%d", column->_sellQuantity);
                NextCell(PFWiseNetBookColumnIndex_AVGSELL, "%0.2f", column->AverageSellPrice);
                NextCell(PFWiseNetBookColumnIndex_TOTAL, "%d", column->TotalQty, UpDownColor(column->TotalQty));
                NextCell(PFWiseNetBookColumnIndex_NETINVEST, "%.2f", column->NetInvestment, UpDownColor(column->NetInvestment));
                NextCell(PFWiseNetBookColumnIndex_MTM, "%d", column->MTM, UpDownColor(column->MTM));
                NextCell(PFWiseNetBookColumnIndex_LTP, "%.2f", column->Self->_ltp, UpDownColor(column->Self->_color._ltp));
                NextCell(PFWiseNetBookColumnIndex_PNL, "%d", column->PNL, UpDownColor(column->PNL));
            }
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::TextColored(UpDownColor(_netPNL), "| Net PNL : %.2f |", _netPNL);
}

void Position::DrawGreekNetBook() {
    float                                    M2M   = 0;
    float                                    Theta = 0;
    float                                    Vega  = 0;
    std::unordered_map<std::string, DValueT> ValueT;

    for (auto& [key, column] : _greekBookContainer) {
        DValueT DValue;
        float   Qty = (column->_buyQuantity - column->_sellQuantity);
        float   LTP = column->Greeks->_future->_ltp;
        DValue._ltp = LTP;
        if (column->_buyQuantity > column->_sellQuantity) {
            DValue._mtm = (Qty) * (LTP - column->AverageBuyPrice);
        } else if (column->_buyQuantity < column->_sellQuantity) {
            DValue._mtm = (-Qty) * (column->AverageSellPrice - LTP);
        }

        DValue._delta = Qty * column->Greeks->_delta;
        DValue._gamma = Qty * column->Greeks->_gamma;
        DValue._vega  = Qty * column->Greeks->_vega;
        DValue._theta = Qty * column->Greeks->_theta;

        M2M += DValue._mtm;
        Theta += DValue._theta;
        Vega += DValue._vega;

        auto iterator = ValueT.find(column->Symbol);
        if (iterator != ValueT.end()) {
            auto& value = iterator->second;
            value._delta += DValue._delta;
            value._gamma += DValue._gamma;
            value._vega += DValue._vega;
            value._theta += DValue._theta;
            value._mtm += DValue._mtm;
            value._value = value._ltp * value._delta;
        } else {
            DValue._symbol = column->Symbol;
            ValueT.emplace(column->Symbol, DValue);
        }
    }

    const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

    if (ImGui::BeginTable("Greek Book Table", GreekBookColumnIndex_END, TableFlags, ImVec2(-FLT_MIN, -frameHeight))) {
        ImGui::TableSetupScrollFreeze(0, 0);

        for (const auto& name : GreekBookColumnName) {
            ImGui::TableSetupColumn(name, TableColumnFlags);
        }
        ImGui::TableHeadersRow();

        _clipper.Begin(ValueT.size());
        while (_clipper.Step()) {
            auto begin = ValueT.begin();
            std::ranges::advance(begin, _clipper.DisplayStart);
            auto end = begin;
            std::ranges::advance(end, _clipper.DisplayEnd - _clipper.DisplayStart);
            for (auto iterator = begin; iterator != end; ++iterator) {
                ImGui::TableNextRow();
                const auto& value = iterator->second;
                NextCell(GreekBookColumnIndex_SYMBOL, "%s", iterator->first.data());
                NextCell(GreekBookColumnIndex_DELTA, "%.2f", value._delta, UpDownColor(value._delta));
                NextCell(GreekBookColumnIndex_GAMMA, "%.2f", value._gamma, UpDownColor(value._gamma));
                NextCell(GreekBookColumnIndex_VEGA, "%.2f", value._vega, UpDownColor(value._vega));
                NextCell(GreekBookColumnIndex_THETA, "%.2f", value._theta, UpDownColor(value._theta));
                NextCell(GreekBookColumnIndex_MTM, "%.2f", value._mtm, UpDownColor(value._mtm));
                NextCell(GreekBookColumnIndex_MARKETRATE, "%.2f", value._ltp, UpDownColor(value._ltp));
                NextCell(GreekBookColumnIndex_VALUE, "%.2f", value._value, UpDownColor(value._value));
            }
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::TextColored(UpDownColor(M2M), "| MTM : %.4f |", M2M);
    ImGui::SameLine();
    ImGui::TextColored(UpDownColor(Theta), "| Theta : %.4f |", Theta);
    ImGui::SameLine();
    ImGui::TextColored(UpDownColor(Vega), "| Vega : %.4f |", Vega);
}

void Position::UpdateGreekValue() {
    if (_calculation == NetBookCalculation_GREEK) {
        for (const GreeksPtrT& column : _greekList) {
            if (column->_isFuture) continue;
            double ExpiryGap = std::abs(Greeks::GetExpiryGap(column->_expiry));
            float  LTP       = column->_isCall ? (column->_future->_bid[0]._price ? column->_future->_bid[0]._price : column->_future->_ltp)
                                               : (column->_future->_ask[0]._price ? column->_future->_ask[0]._price : column->_future->_ltp);

            column->_impliedVol = Greeks::GetIV(LTP, column->_strike, 0, ExpiryGap, column->_self->_ltp, column->_isCall);
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
    _timer.async_wait([&](const boost::system::error_code& err) { UpdateGreekValue(); });
}
