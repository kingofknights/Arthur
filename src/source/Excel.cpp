//
// Created by VIKLOD on 10-04-2023.
//
#include "../include/Excel.hpp"

#include <nlohmann/json.hpp>

#include "../API/ContractInfo.hpp"
#include "../Excel/ExcelAutomation.hpp"
#include "../include/Configuration.hpp"
#include "../include/Logger.hpp"
#include "../include/Utils.hpp"

static ExcelSheetItemContainerT ExcelSheetItemContainer;

#define EXCEL_CONFIG_PATH "Config/Excel.json"
extern AllContractT AllContract;

void DrawCollapseHeader(const std::string& name_, ExcelSheetItemT& sheet_, std::string& contract_) {
	if (ImGui::CollapsingHeader(name_.data())) {
		ImGui::Indent();
		sheet_.Filter.Draw("Contract");
		if (sheet_.Filter.IsActive()) {
			ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
			if (ImGui::Begin("Contract Filter", nullptr, OverlayFlags)) {
				Utils::ContractFilter(sheet_.Filter, contract_);
			}

			ImGui::End();
			ImGui::SameLine();
			if (ImGui::Button("Add Contract")) {
				auto token = ContractInfo::GetToken(contract_);

				ExcelContactItemT ExcelContactItem{.Index = sheet_.ExcelContactItemContainer.size(), .Self = ContractInfo::GetLiveDataRef(token)};
				ExcelAutomation::SetValue(ExcelContactItem.Index, sheet_.Sheets, contract_);
				if (ExcelContactItem.Self) {
					sheet_.ExcelContactItemContainer.emplace(contract_, ExcelContactItem);
				}
				sheet_.Filter.Clear();
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear")) {
				sheet_.Filter.Clear();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Save")) {
			std::string currentPath = std::filesystem::current_path().append("Automation").string();
			ExcelAutomation::Save(sheet_, currentPath);
		}

		for (const ExcelContactItemContainerT::value_type& valueType : sheet_.ExcelContactItemContainer) {
			ImGui::BulletText("%zu %s", valueType.second.Index, valueType.first.data());
		}
		ImGui::Unindent();
	}
}

ExcelWindow::ExcelWindow() { Imports(EXCEL_CONFIG_PATH); }

void ExcelWindow::paint(bool* show_) {
	if (*show_) {
		DrawExcelWindow(show_);
	}
}

void ExcelWindow::Update(int token_) {
	for (const auto& [_, sheet_] : ExcelSheetItemContainer) {
		for (const auto& item_ : sheet_.ExcelContactItemContainer) {
			if (item_.second.Self->Token == token_) ExcelAutomation::UpdateValue(item_.second.Index, sheet_.Sheets, item_.second.Self);
		}
	}
}

ExcelWindow::~ExcelWindow() {
	Exports(EXCEL_CONFIG_PATH);
	std::string currentPath = std::filesystem::current_path().append("Automation").string();
	for (ExcelSheetItemContainerT::value_type& valueType : ExcelSheetItemContainer) {
		ExcelAutomation::Save(valueType.second, currentPath);
	}
}

void ExcelWindow::DrawExcelWindow(bool* show_) {
	if (ImGui::Begin("Excel Automation", show_)) {
		const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("Excels List", ImVec2(-FLT_MIN, -frameHeight))) {
			for (ExcelSheetItemContainerT::value_type& valueType_ : ExcelSheetItemContainer) {
				const auto& name = valueType_.first;
				ImGui::PushID(name.data());
				DrawCollapseHeader(name, valueType_.second, _contract);
				ImGui::PopID();
			}
		}
		ImGui::EndChild();
		ImGui::Separator();
		ImGui::InputTextWithHint("##Sheet Name", "Enter sheet name", _sheetName, STRATEGY_NAME_LENGTH);
		ImGui::SameLine();
		ImGui::BeginDisabled(not strlen(_sheetName));
		if (ImGui::Button("Add Sheet")) {
			bool found = std::any_of(ExcelSheetItemContainer.begin(), ExcelSheetItemContainer.end(), [&](const auto& valueType) { return valueType.first == _sheetName; });

			if (not found) {
				Excel::_WorksheetPtr workbooks = ExcelAutomation::Instance()->AddSheets(_sheetName);
				ExcelSheetItemT		 ExcelSheetItem;
				ExcelSheetItem.Sheets = workbooks;
				ExcelSheetItem.Name	  = std::string(_sheetName);

				ExcelSheetItemContainer.emplace(ExcelSheetItem.Name, ExcelSheetItem);
				std::memset(_sheetName, '\0', STRATEGY_NAME_LENGTH);
			}
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button(fmt::format("{} Excel##Excel Application", _showExcelApplication ? "Close" : "Show").data())) {
			_showExcelApplication = !_showExcelApplication;
			ExcelAutomation::Instance()->SetExcelVisibility(_showExcelApplication);
		}
	}
	ImGui::End();
}

void ExcelWindow::Imports(std::string_view path_) {
	std::fstream file(path_.data(), std::ios::in);
	if (not file.is_open()) return;

	nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);
	for (const auto& [key, value] : root.items()) {
		std::string path = std::filesystem::current_path().append("Automation").append(key).string().append(".xlsx");

		Excel::_WorksheetPtr workbooks;
		if (std::filesystem::exists(path)) {
			workbooks = ExcelAutomation::Instance()->OpenSheets(path, key);
		} else {
			workbooks = ExcelAutomation::Instance()->AddSheets(key);
		}
		ExcelSheetItemT ExcelSheetItem;
		ExcelSheetItem.Sheets = workbooks;
		ExcelSheetItem.Name	  = key;

		for (const auto& [contract, index] : value.items()) {
			auto token = ContractInfo::GetToken(contract);

			ExcelContactItemT ExcelContactItem{.Index = ExcelSheetItem.ExcelContactItemContainer.size(), .Self = ContractInfo::GetLiveDataRef(token)};
			if (ExcelContactItem.Self) {
				ExcelSheetItem.ExcelContactItemContainer.emplace(contract, ExcelContactItem);
			}
		}

		ExcelSheetItemContainer.emplace(ExcelSheetItem.Name, ExcelSheetItem);
	}

	file.close();
}

void ExcelWindow::Exports(std::string_view path_) {
	if (ExcelSheetItemContainer.empty()) {
		std::remove(path_.data());
		return;
	}

	nlohmann::ordered_json root;
	std::ranges::for_each(ExcelSheetItemContainer, [&](const ExcelSheetItemContainerT::value_type& valueType_) {
		nlohmann::ordered_json item;
		for (const ExcelContactItemContainerT::value_type& valueType : valueType_.second.ExcelContactItemContainer) {
			item[valueType.first] = valueType.second.Index;
		}
		root[valueType_.first] = item;
	});

	std::fstream file(path_.data(), std::ios::out | std::ios::trunc);
	if (not file.is_open()) return;
	file << root.dump();
	file.close();
}
