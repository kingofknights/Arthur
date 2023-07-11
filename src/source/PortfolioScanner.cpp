//
// Created by VIKLOD on 24-04-2023.
//
#include "../include/PortfolioScanner.hpp"

#include <nlohmann/json.hpp>

#include "../Knight/Scanner.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/Portfolio.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"

extern std::string						StatusDisplay;
extern GlobalPortfolioScannerContainerT GlobalPortfolioScannerContainer;

PortfolioScanner::PortfolioScanner(const std::string &strategyName_) : _hasParameter(false), _strategyID(INT_MIN), _strategyName(strategyName_), _selectedParam(0) {
	char variable = 'A';
	for (auto &column : ScannerFunctionList) {
		ScannerFunctionInfoT info{.Selected = false, .Variable = variable++, .Name = column};
		_scannerFunctionListContainer.push_back(info);
	}
	LoadParameter();
}

void PortfolioScanner::paint(bool *show_) {
	if (not *show_) return;
	if (_hasParameter and _strategyID != INT_MIN) {
		ScannerWindow(show_);
	} else {
		StatusDisplay = "Strategy not found";
	}
}
void PortfolioScanner::LoadParameter() {
	{
		LOG(INFO, "{} {} {}", __FUNCTION__, GetStrategyID_, _strategyName)
		auto table = Lancelot::ContractInfo::GetResultWithIndex(FORMAT(GetStrategyID_, _strategyName));
		if (not table.empty() and not table[0].empty()) {
			_strategyID = std::stoi(table[0][0]);
		}
	}
	{
		LOG(INFO, "{} {} {}", __FUNCTION__, GetStrategyParams_, _strategyName)
		auto table = Lancelot::ContractInfo::GetResultWithIndex(FORMAT(GetStrategyParams_, _strategyName));
		for (auto &row : table) {
			_scannerInfoFromDatabase.emplace(std::stoi(row[0]), row[1]);
		}
		_hasParameter = not _scannerInfoFromDatabase.empty();
	}
}

void PortfolioScanner::ScannerWindow(bool *show_) {
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal(SCANNER_WINDOW, show_)) {
		ImGui::Columns(3);
		FirstColumn();
		ImGui::NextColumn();
		SecondColumn();
		ImGui::NextColumn();
		ThirdColumn();
		ImGui::EndPopup();
	}
}
void PortfolioScanner::FirstColumn() {
	if (ImGui::BeginTable("Function Table", ScannerFunctionColumnIndex_END, TableFlags)) {
		for (const auto &column : ScannerTableColumnName) {
			ImGui::TableSetupColumn(column, TableColumnFlags | ImGuiTableColumnFlags_WidthStretch);
		}
		ImGui::TableHeadersRow();
		for (auto &column : _scannerFunctionListContainer) {
			ImGui::TableNextRow();
			ImGui::PushID(column.Variable);
			FirstCellWithPadding(ScannerFunctionColumnIndex_NAME, "%s", column.Name.data());
			ImGui::TableSetColumnIndex(ScannerFunctionColumnIndex_VARIABLE);
			ImGui::Checkbox(fmt::format("{}", column.Variable).data(), &column.Selected);
			ImGui::PopID();
		}
		ImGui::EndTable();
	}
}
void PortfolioScanner::SecondColumn() {
	ImVec2		size	= ImGui::GetContentRegionAvail();
	const float spacing = ImGui::GetStyle().ItemSpacing.y;

	if (ImGui::BeginChild("Selected Functions", ImVec2(size.x, size.y / 2 - spacing), true)) {
		for (auto &column : _scannerFunctionListContainer) {
			if (column.Selected) {
				ImGui::Text("%c = %s", column.Variable, column.Name.data());
			}
		}
	}
	ImGui::EndChild();

	if (ImGui::BeginChild("Formula", ImVec2(size.x, size.y / 2 - spacing), true)) {
		const float frameHeight = ImGui::GetFrameHeightWithSpacing() * 3;
		ImGui::InputTextMultiline("##Equations", &_equations, ImVec2(size.x, size.y / 2 - frameHeight));
		ImGui::Separator();
		ImGui::SetNextItemWidth(size.x);
		ImGui::InputTextWithHint("##Name", "Enter Formula Name", &_formulaName);
		ImGui::BeginDisabled(_formulaName.empty());
		if (ImGui::Button("Create", ImVec2(size.x, 0))) {
			CreateFormula();
		}
		ImGui::EndDisabled();
	}
	ImGui::EndChild();
}
void PortfolioScanner::ThirdColumn() {
	ImVec2		size		= ImGui::GetContentRegionAvail();
	const float frameHeight = ImGui::GetStyle().ItemSpacing.y;

	if (ImGui::BeginChild("ScannerAPI", ImVec2(size.x, size.y / 2 - frameHeight), true)) {
		ImGui::Text("%s", fmt::format("ScannerAPI({}, {}, {})", "uniqueID", _strategyID, _selectedParam).data());
		ImGui::Separator();
		for (const auto &item : _scannerInfoFromDatabase) {
			if (ImGui::Selectable(fmt::format("{} {}", item.first, item.second).data(), _selectedParam == item.first)) {
				_selectedParam = item.first;
			}
		}
	}
	ImGui::EndChild();
	if (_deleteScannerID != -1) {
		_scannerSaveContainer.erase(_scannerSaveContainer.begin() + _deleteScannerID);
		_deleteScannerID = -1;
	}
	if (ImGui::BeginChild("Saved  Functions", ImVec2(size.x, size.y / 2 - frameHeight), true)) {
		if (ImGui::BeginTable("Saved Functions", ScannerSavedColumnIndex_END, TableFlags)) {
			for (auto &column : ScannerSavedTableColumnName) {
				ImGui::TableSetupColumn(column, TableColumnFlags | ImGuiTableColumnFlags_WidthStretch);
			}
			ImGui::TableHeadersRow();
			int i = 0;
			for (SaveScannerItemT &item : _scannerSaveContainer) {
				ImGui::PushID(item.Name.data());
				ImGui::TableNextRow();

				FirstCell(ScannerSavedColumnIndex_NUMBER, fmt::format("#{}", i).data(), _selectedScanner, i);

				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("%s", item.ExpandedEquation.data());
					ImGui::EndTooltip();
				}
				if (_selectedScanner == i) {
					if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
						_deleteScannerID = i;
					}
				}
				NextCell(ScannerSavedColumnIndex_NAME, "%s", item.Name.data());
				ImGui::TableSetColumnIndex(ScannerSavedColumnIndex_OPERATIONS);

				if (ImGui::Button(fmt::format("{} {}##Operations", item.Applied ? ICON_MD_STOP : ICON_MD_PLAY_ARROW, item.Applied ? "Stop" : "Apply").data(), ImVec2(-FLT_MIN, 0))) {
					if (not item.Applied) {
						if (not _strategyList.empty()) {
							ScannerResultOutputT ScannerResultOutput{.PortfolioPtr		= std::static_pointer_cast<Portfolio>(shared_from_this()),
																	 .ParameterInfoList = _strategyList.front()->ParameterInfoList};
							GlobalPortfolioScannerContainer.insert_or_assign(item.UniqueID, ScannerResultOutput);
						}

						item.Applied = Scanner::GetInstance().EvaluateExp(item.UniqueID, item.ExpandedEquation, false);
					} else {
						Scanner::GetInstance().ScannerUnsubscribe(item.UniqueID);
						item.Applied = false;
					}
				}

				ImGui::PopID();
				++i;
			}
			ImGui::EndTable();
		}
	}
	ImGui::EndChild();
}
void PortfolioScanner::CreateFormula() {
	std::stringstream ss;
	for (const auto &item : _scannerFunctionListContainer) {
		if (item.Selected) {
			ss << fmt::format("var {} := {}(token_);", item.Variable, item.Name, item.Variable);
		}
	}
	int64_t uniqueID = time(nullptr);
	ss << fmt::format("var output := if(({}), ScannerAPI({}, {}, {}, token_), 0);\n", _equations, uniqueID, _strategyID, _selectedParam);
	ss << "output";

	_unfoldedFormula = ss.str();

	_scannerSaveContainer.emplace_back(SaveScannerItemT{.Applied = false, .UniqueID = uniqueID, .Name = _formulaName, .ExpandedEquation = _unfoldedFormula});
}
void PortfolioScanner::Export(std::string_view path_) {
	if (_scannerSaveContainer.empty()) {
		std::remove(path_.data());
		return;
	}

	nlohmann::ordered_json root;
	for (const auto &ScannerItem : _scannerSaveContainer) {
		nlohmann::json item;
		item["ID"]				 = ScannerItem.UniqueID;
		item["Name"]			 = ScannerItem.Name;
		item["ExpandedEquation"] = ScannerItem.ExpandedEquation;
		LOG(INFO, "Exporting Scanner {} {}", ScannerItem.UniqueID, ScannerItem.Name)
		root.emplace_back(item);
	}
	LOG(INFO, "{} {}", __FUNCTION__, "Done")
	std::fstream file(path_.data(), std::ios::trunc | std::ios::out);
	if (not file.is_open()) {
		return;
	}
	file << root.dump();
	file.close();
}
void PortfolioScanner::Import(std::string_view path_) {
	std::fstream file(path_.data(), std::ios::in);
	if (not file.is_open()) return;

	nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);
	std::ranges::for_each(root.items(), [&](const auto &valueType_) {
		auto		value			 = valueType_.value();
		int64_t		uniqueID		 = value.at("ID").template get<int64_t>();
		std::string name			 = value.at("Name").template get<std::string>();
		std::string expandedEquation = value.at("ExpandedEquation").template get<std::string>();
		LOG(INFO, "Importing Scanner {} {}", uniqueID, name)
		_scannerSaveContainer.emplace_back(SaveScannerItemT{.Applied = false, .UniqueID = uniqueID, .Name = name, .ExpandedEquation = expandedEquation});
	});
	LOG(INFO, "{} {}", __FUNCTION__, "Done")
	file.close();
}
