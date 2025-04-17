//
// Created by VIKLOD on 07-03-2023.
//

#pragma once

#include "Structure.hpp"

#include <optional>

enum StrategyStatus : int;

class Utils {
  public:
    static auto GetStrategyRow(uint32_t pf_) -> std::optional<WeakStrategyRowPtrT>;

    static auto FormatTimeToString(uint64_t time_) -> std::string;

    static void StatusBar();

    static void RemovePortfolio();

    static void CreateSupportFolder();

    static void GetClientList(int userId_);

    static void GetAllContractCallback(Lancelot::ResultSetPtrT result_, float ltp_, float low_, float high_);

    static void ResetPortfolio(StrategyStatus status_);

    static auto ToggleMenuItem(std::string_view window_, bool& open_) -> bool;

    static void DrawTradeRow(const OrderInfoPtrT& tradeInfo_, int& first_, int second_);

    static void AppendPortfolio(uint32_t pf_, WeakStrategyRowPtrT ptr_);

    static void ContractFilter(ImGuiTextFilter& filter_, std::string& index_);

    static auto ScannerAPI(double pf_, double name_, double params_, double token_) -> double;

    static auto ManualSerialize(const OrderFormInfoT& manualOrderInfo_) -> std::string;

    static auto CancelOrderSerialize(const OrderInfoPtrT& orderInfo_) -> std::string;

    static auto StrategySerialize(const StrategyRowPtrT& row_, const std::string& name_, Lancelot::RequestType type_) -> std::string;

  private:
    static GlobalStrategyListT GlobalStrategyList;
};
