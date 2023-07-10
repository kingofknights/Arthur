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

template <typename Type>
void UpdateTradeInfoNetbook(Type& data, const OrderInfoPtrT& tradeInfo_) {
    if (tradeInfo_->Side == Lancelot::Side_BUY) {
        data->TotalBuyPrice += tradeInfo_->Price * tradeInfo_->Quantity;
        data->BuyQuantity += tradeInfo_->Quantity;
        data->AverageBuyPrice = data->TotalBuyPrice / data->BuyQuantity;
    } else {
        data->TotalSellPrice += tradeInfo_->Price * tradeInfo_->Quantity;
        data->SellQuantity += tradeInfo_->Quantity;
        data->AverageSellPrice = data->TotalSellPrice / data->SellQuantity;
    }
}

template <typename Container, typename Sequencial, typename Key>
void UpdateNetBook(Container& container_, Sequencial& sequencial_, Key key_, const OrderInfoPtrT& tradeInfo_) {
    auto iterator = container_.find(key_);
    if (iterator != container_.end()) {
        NetBookColumnPtrT& data = iterator->second;
        UpdateTradeInfoNetbook(data, tradeInfo_);
        data->NetInvestment = data->TotalBuyPrice - data->TotalSellPrice;
        data->TotalQty		= data->BuyQuantity - data->SellQuantity;
    } else {
        NetBookColumnPtrT data = std::make_shared<NetBookColumnT>();
        data->AverageBuyPrice  = 0;
        data->AverageSellPrice = 0;
        data->BuyQuantity	   = 0;
        data->SellQuantity	   = 0;
        data->TotalBuyPrice	   = 0;
        data->TotalSellPrice   = 0;
        data->LastLTP		   = 0;
        data->LastPNL		   = 0;
        data->PF			   = tradeInfo_->PF;
        data->Self			   = ContractInfo::GetLiveDataRef(tradeInfo_->Token);
        UpdateTradeInfoNetbook(data, tradeInfo_);
        data->NetInvestment = data->TotalBuyPrice - data->TotalSellPrice;
        data->TotalQty		= data->BuyQuantity - data->SellQuantity;

        container_.emplace(key_, data);
        sequencial_.push_back(std::make_pair(key_, data));
        std::sort(sequencial_.begin(), sequencial_.end());
    }
}

void Position::DrawBook(bool* show_) {
    if (ImGui::Begin("Position", show_, ImGuiWindowFlags_NoTitleBar)) {
        if (ImGui::BeginTabBar("Position Canvas", StrategyWorkspaceTabFlags)) {
            if (ImGui::BeginTabItem("Symbol Position")) {
                _calculation = NetBookCalculation_SYMBOL;
                DrawSymbolWiseNetBook();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Strategy Position")) {
                _calculation = NetBookCalculation_PF;
                DrawPFWiseNetBook();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Greeks")) {
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
    UpdateNetBook(_symbolWiseTradeContainer, _symbolWiseTradeContainerVec, tradeInfo_->Token, tradeInfo_);
}

void Position::PFWiseBookUpdate(const OrderInfoPtrT& tradeInfo_) {
    std::pair<int, int> key = std::make_pair(tradeInfo_->PF, tradeInfo_->Token);
    UpdateNetBook(_pFWiseTradeContainer, _pFWiseTradeContainerVec, key, tradeInfo_);
}

void Position::GreekBookUpdate(const OrderInfoPtrT& tradeInfo_) {
    auto iteratorContainer = _greekBookContainer.find(tradeInfo_->Token);
    if (iteratorContainer != _greekBookContainer.end()) {
        GreekBookColumnPtrT& data = iteratorContainer->second;
        UpdateTradeInfoNetbook(data, tradeInfo_);

    } else {
        GreekBookColumnPtrT data = std::make_shared<GreekBookColumnT>();

        GreeksPtrT greek = std::make_shared<GreeksT>();
        {
            auto* resultSet	   = Lancelot::ContractInfo::GetResultSet(tradeInfo_->Token);
            greek->IsCall	   = resultSet->_option == Lancelot::OptionType_CALL;
            greek->IsFuture	   = resultSet->_instType == Lancelot::Instrument_FUTURE;
            greek->Expiry	   = resultSet->_expiryDate;
            greek->StrikePrice = resultSet->_strikePrice;
            greek->IV		   = 1;
            greek->Delta	   = 1;
            greek->Gamma	   = 1;
            greek->Vega		   = 1;
            greek->Theta	   = 1;
            greek->Self		   = ContractInfo::GetLiveDataRef(tradeInfo_->Token);
            greek->Future	   = ContractInfo::GetLiveDataRef(Lancelot::ContractInfo::GetFuture(tradeInfo_->Token));
            _greekList.push_back(greek);
        }

        _greekList.push_back(greek);

        data->Symbol		   = Lancelot::ContractInfo::GetSymbol(tradeInfo_->Token);
        data->BuyQuantity	   = 0;
        data->SellQuantity	   = 0;
        data->TotalSellPrice   = 0;
        data->TotalBuyPrice	   = 0;
        data->AverageSellPrice = 0;
        data->AverageBuyPrice  = 0;
        data->Greeks		   = greek;

        UpdateTradeInfoNetbook(data, tradeInfo_);
        _greekBookContainer.emplace(tradeInfo_->Token, data);
    }
}

double Position::CalculateSymbolWisePNL(const NetBookColumnPtrT& column) {
    if (column->LastLTP == column->Self->LastTradePrice) {
        return column->LastPNL;
    }
    column->LastLTP = column->Self->LastTradePrice;

    double MTM = 0;
    float  BPL = std::min(column->BuyQuantity, column->SellQuantity) * (column->AverageSellPrice - column->AverageBuyPrice);
    if (column->BuyQuantity > column->SellQuantity) {
        MTM = (column->BuyQuantity - column->SellQuantity) * (column->LastLTP - column->AverageBuyPrice);
    } else if (column->BuyQuantity < column->SellQuantity) {
        MTM = (column->SellQuantity - column->BuyQuantity) * (column->AverageSellPrice - column->LastLTP);
    } else {
        BPL = column->TotalSellPrice - column->TotalBuyPrice;
    }

    double PNL		= MTM + BPL;
    column->MTM		= MTM;
    column->PNL		= PNL;
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
                NextCell(SymbolWiseNetBookColumnIndex_CONTRACT, "%s", column->Self->Description.data());
                NextCell(SymbolWiseNetBookColumnIndex_AVGBID, "%0.2f", column->AverageBuyPrice);
                NextCell(SymbolWiseNetBookColumnIndex_BUYQTY, "%d", column->BuyQuantity);
                NextCell(SymbolWiseNetBookColumnIndex_SELLQTY, "%d", column->SellQuantity);
                NextCell(SymbolWiseNetBookColumnIndex_AVGSELL, "%0.2f", column->AverageSellPrice);
                NextCell(SymbolWiseNetBookColumnIndex_TOTAL, "%d", column->TotalQty, UpDownColor(column->TotalQty));
                NextCell(SymbolWiseNetBookColumnIndex_NETINVEST, "%.2f", column->NetInvestment, UpDownColor(column->NetInvestment));
                NextCell(SymbolWiseNetBookColumnIndex_MTM, "%d", column->MTM, UpDownColor(column->MTM));
                NextCell(SymbolWiseNetBookColumnIndex_LTP, "%.2f", column->Self->LastTradePrice, UpDownColor(column->Self->Color.LTP));
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
                NextCell(PFWiseNetBookColumnIndex_CONTRACT, "%s", column->Self->Description.data());
                NextCell(PFWiseNetBookColumnIndex_AVGBID, "%0.2f", column->AverageBuyPrice);
                NextCell(PFWiseNetBookColumnIndex_BUYQTY, "%d", column->BuyQuantity);
                NextCell(PFWiseNetBookColumnIndex_SELLQTY, "%d", column->SellQuantity);
                NextCell(PFWiseNetBookColumnIndex_AVGSELL, "%0.2f", column->AverageSellPrice);
                NextCell(PFWiseNetBookColumnIndex_TOTAL, "%d", column->TotalQty, UpDownColor(column->TotalQty));
                NextCell(PFWiseNetBookColumnIndex_NETINVEST, "%.2f", column->NetInvestment, UpDownColor(column->NetInvestment));
                NextCell(PFWiseNetBookColumnIndex_MTM, "%d", column->MTM, UpDownColor(column->MTM));
                NextCell(PFWiseNetBookColumnIndex_LTP, "%.2f", column->Self->LastTradePrice, UpDownColor(column->Self->Color.LTP));
                NextCell(PFWiseNetBookColumnIndex_PNL, "%d", column->PNL, UpDownColor(column->PNL));
            }
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::TextColored(UpDownColor(_netPNL), "| Net PNL : %.2f |", _netPNL);
}

void Position::DrawGreekNetBook() {
    float									 M2M   = 0;
    float									 Theta = 0;
    float									 Vega  = 0;
    std::unordered_map<std::string, DValueT> ValueT;

    for (auto& [key, column] : _greekBookContainer) {
        DValueT DValue;
        float	Qty		  = (column->BuyQuantity - column->SellQuantity);
        float	LTP		  = column->Greeks->Future->LastTradePrice;
        DValue.MarketRate = LTP;
        if (column->BuyQuantity > column->SellQuantity) {
            DValue.MTM = (Qty) * (LTP - column->AverageBuyPrice);
        } else if (column->BuyQuantity < column->SellQuantity) {
            DValue.MTM = (-Qty) * (column->AverageSellPrice - LTP);
        }

        DValue.Delta = Qty * column->Greeks->Delta;
        DValue.Gamma = Qty * column->Greeks->Gamma;
        DValue.Vega	 = Qty * column->Greeks->Vega;
        DValue.Theta = Qty * column->Greeks->Theta;

        M2M += DValue.MTM;
        Theta += DValue.Theta;
        Vega += DValue.Vega;

        auto iterator = ValueT.find(column->Symbol);
        if (iterator != ValueT.end()) {
            auto& value = iterator->second;
            value.Delta += DValue.Delta;
            value.Gamma += DValue.Gamma;
            value.Vega += DValue.Vega;
            value.Theta += DValue.Theta;
            value.MTM += DValue.MTM;
            value.Value = value.MarketRate * value.Delta;
        } else {
            DValue.Symbol = column->Symbol;
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
                NextCell(GreekBookColumnIndex_DELTA, "%.2f", value.Delta, UpDownColor(value.Delta));
                NextCell(GreekBookColumnIndex_GAMMA, "%.2f", value.Gamma, UpDownColor(value.Gamma));
                NextCell(GreekBookColumnIndex_VEGA, "%.2f", value.Vega, UpDownColor(value.Vega));
                NextCell(GreekBookColumnIndex_THETA, "%.2f", value.Theta, UpDownColor(value.Theta));
                NextCell(GreekBookColumnIndex_MTM, "%.2f", value.MTM, UpDownColor(value.MTM));
                NextCell(GreekBookColumnIndex_MARKETRATE, "%.2f", value.MarketRate, UpDownColor(value.MarketRate));
                NextCell(GreekBookColumnIndex_VALUE, "%.2f", value.Value, UpDownColor(value.Value));
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
            if (column->IsFuture) continue;
            double ExpiryGap = std::abs(Greeks::GetExpiryGap(column->Expiry));
            float  LTP		 = column->IsCall ? (column->Future->Bid[0].Price ? column->Future->Bid[0].Price : column->Future->LastTradePrice)
                                              : (column->Future->Ask[0].Price ? column->Future->Ask[0].Price : column->Future->LastTradePrice);

            column->IV	  = Greeks::GetIV(LTP, column->StrikePrice, 0, ExpiryGap, column->Self->LastTradePrice, column->IsCall);
            column->Delta = Greeks::GetDelta(LTP, column->StrikePrice, column->IV, 0, ExpiryGap, column->IsCall);
            column->Gamma = Greeks::GetGamma(LTP, column->StrikePrice, column->IV, 0, ExpiryGap, column->IsCall);
            column->Vega  = Greeks::GetVega(LTP, column->StrikePrice, column->IV, 0, ExpiryGap, column->IsCall);
            column->Theta = Greeks::GetTheta(LTP, column->StrikePrice, column->IV, 0, ExpiryGap, column->IsCall);
        }
    } else if (_calculation == NetBookCalculation_SYMBOL) {
        auto copy = _symbolWiseTradeContainerVec;
        _netPNL	  = std::accumulate(copy.begin(), copy.end(), 0, [&](double sum, const auto& pair_) { return sum + CalculateSymbolWisePNL(pair_.second); });
    } else if (_calculation == NetBookCalculation_PF) {
        auto copy = _pFWiseTradeContainerVec;
        _netPNL	  = std::accumulate(copy.begin(), copy.end(), 0, [&](double sum, const auto& pair_) { return sum + CalculateSymbolWisePNL(pair_.second); });
    }

    TimerEvent();
}

void Position::TimerEvent() {
    _timer.expires_from_now(boost::posix_time::seconds(1));
    _timer.async_wait([&](const boost::system::error_code& err) { UpdateGreekValue(); });
}
