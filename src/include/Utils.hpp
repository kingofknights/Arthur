//
// Created by VIKLOD on 07-03-2023.
//

#pragma once

#include <optional>

#include "Structure.hpp"

enum StrategyStatus : int;
enum RequestType : int;

class Utils {
public:
	static std::optional<WeakStrategyRowPtrT> GetStrategyRow(uint32_t pf_);

	static std::string FormatTimeToString(uint64_t time_);
	static void		   StatusBar();
	static void		   RemovePortfolio();
	static void		   CreateSupportFolder();
	static void		   GetClientList(int userId_);
	static void		   GetAllContractCallback(const Lancelot::ResultSetPtrT result_, float ltp_, float low_, float high_);
	static void		   ResetPortfolio(StrategyStatus status_);
	static bool		   ToggleMenuItem(std::string_view window_, bool &open_);
	static void		   DrawTradeRow(const OrderInfoPtrT &tradeInfo_, int &first_, int second_);
	static void		   AppendPortfolio(uint32_t pf_, WeakStrategyRowPtrT ptr_);
	static void		   ContractFilter(ImGuiTextFilter &filter_, std::string &index_);
	static double	   ScannerAPI(double pf_, double name_, double params_, double token_);
	static std::string manualSerialize(const OrderFormInfoT &manualOrderInfo_);
	static std::string cancelOrderSerialize(const OrderInfoPtrT &orderInfo_);
	static std::string strategySerialize(const StrategyRowPtrT &row_, const std::string &name_, RequestType type_);

private:
	static GlobalStrategyListT GlobalStrategyList;
};
