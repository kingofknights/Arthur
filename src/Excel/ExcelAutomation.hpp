//
// Created by VIKLOD on 10-04-2023.
//
#pragma once

#include <filesystem>
#include <memory>

#include "ExcelHeader.hpp"

class ExcelAutomation {
public:
	static ExcelAutomationPtrT& Instance();

	ExcelAutomation();
	~ExcelAutomation();

	void SetExcelVisibility(bool show_);

	Excel::_WorksheetPtr AddSheets(std::string_view name_);
	Excel::_WorksheetPtr OpenSheets(std::string_view path_, std::string_view name_);

	static void Save(ExcelSheetItemT& excelSheetItem_, std::filesystem::path path_);
	static void SetValue(int index_, Excel::_WorksheetPtr sheet_, std::string_view name_);
	static void UpdateValue(int index_, Excel::_WorksheetPtr sheet_, const MarketWatchDataPtrT& marketWatchData_);

protected:
	void ShowErrorMessage(const std::string& message_);

private:
	Excel::_ApplicationPtr	   _application;
	static ExcelAutomationPtrT _ptr;
};
