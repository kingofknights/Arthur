//
// Created by VIKLOD on 24-04-2023.
//
#include "PortfolioScanner.hpp"

#include "Configuration.hpp"
#include "Enums.hpp"
// #include "Knight/Scanner.hpp"
#include "Portfolio.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <nlohmann/json.hpp>

#include <utility>

extern std::string                      StatusDisplay;
extern GlobalPortfolioScannerContainerT GlobalPortfolioScannerContainer;

PortfolioScanner::PortfolioScanner(std::string strategyName_) : _strategyName(std::move(strategyName_)), _hasParameter(false), _selectedParam(0), _strategyID(INT_MIN) {
    char variable = 'A';
    for (const auto& column : ScannerFunctionList) {
        ScannerFunctionInfoT info{._selected = false, ._variable = variable++, ._name = column};
        _scannerFunctionListContainer.push_back(info);
    }
    LoadParameter();
}

void PortfolioScanner::Paint(bool* show_) {
#ifdef TURNOFF_SCANNER
#else
    if (not*show_) {
        return;
    }
    if (_hasParameter and _strategyID != INT_MIN) {
        ScannerWindow(show_);
    } else {
        StatusDisplay = "Strategy not found";
    }
#endif
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
        for (auto& row : table) {
            _scannerInfoFromDatabase.emplace(std::stoi(row[0]), row[1]);
        }
        _hasParameter = not _scannerInfoFromDatabase.empty();
    }
}

void PortfolioScanner::ScannerWindow(bool* show_) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    if (ImGui::BeginPopupModal(SCANNER_WINDOW, show_)) {
        ImGui::Columns(3);
        FirstColumn();
        ImGui::NextColumn();
        SecondColumn();
        ImGui::NextColumn();
        ThirdColumn();
        ImGui::EndColumns();
        ImGui::EndPopup();
    }
}
void PortfolioScanner::FirstColumn() {
    if (ImGui::BeginTable("Function Table", ScannerFunctionColumnIndex_END, TableFlags)) {
        for (const auto& column : ScannerTableColumnName) {
            ImGui::TableSetupColumn(column, TableColumnFlags | ImGuiTableColumnFlags_WidthStretch);
        }
        ImGui::TableHeadersRow();
        for (auto& column : _scannerFunctionListContainer) {
            ImGui::TableNextRow();
            ImGui::PushID(column._variable);
            FirstCellWithPadding(ScannerFunctionColumnIndex_NAME, column._name);
            ImGui::TableSetColumnIndex(ScannerFunctionColumnIndex_VARIABLE);
            ImGui::Checkbox(FORMAT("{}", column._variable).data(), &column._selected);
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}
void PortfolioScanner::SecondColumn() {
    const ImVec2 size    = ImGui::GetContentRegionAvail();
    const float  spacing = ImGui::GetStyle().ItemSpacing.y;

    if (ImGui::BeginChild("Selected Functions", ImVec2(size.x, (size.y / 2) - spacing), ImGuiChildFlags_Borders)) {
        for (auto& column : _scannerFunctionListContainer) {
            if (column._selected) {
                ImGui::Text("%c = %s", column._variable, column._name.data());
            }
        }
    }
    ImGui::EndChild();

    if (ImGui::BeginChild("Formula", ImVec2(size.x, (size.y / 2) - spacing), ImGuiChildFlags_Borders)) {
        const float frameHeight = ImGui::GetFrameHeightWithSpacing() * 3;
        ImGui::InputTextMultiline("##Equations", &_equations, ImVec2(size.x, (size.y / 2) - frameHeight));
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
    const ImVec2 size        = ImGui::GetContentRegionAvail();
    const float  frameHeight = ImGui::GetStyle().ItemSpacing.y;

    if (ImGui::BeginChild("ScannerAPI", ImVec2(size.x, (size.y / 2) - frameHeight), ImGuiChildFlags_Borders)) {
        ImGui::Text("%s", FORMAT("ScannerAPI({}, {}, {})", "uniqueID", _strategyID, _selectedParam).data());
        ImGui::Separator();
        for (const auto& item : _scannerInfoFromDatabase) {
            if (ImGui::Selectable(FORMAT("{} {}", item.first, item.second).data(), _selectedParam == item.first)) {
                _selectedParam = item.first;
            }
        }
    }
    ImGui::EndChild();
    if (_deleteScannerID != -1) {
        _scannerSaveContainer.erase(_scannerSaveContainer.begin() + _deleteScannerID);
        _deleteScannerID = -1;
    }
    if (ImGui::BeginChild("Saved  Functions", ImVec2(size.x, (size.y / 2) - frameHeight), ImGuiChildFlags_Borders)) {
        if (ImGui::BeginTable("Saved Functions", ScannerSavedColumnIndex_END, TableFlags)) {
            for (auto& column : ScannerSavedTableColumnName) {
                ImGui::TableSetupColumn(column, TableColumnFlags | ImGuiTableColumnFlags_WidthStretch);
            }
            ImGui::TableHeadersRow();
            int i = 0;
            for (SaveScannerItemT& item : _scannerSaveContainer) {
                ImGui::PushID(item._name.data());
                ImGui::TableNextRow();

                FirstCell(ScannerSavedColumnIndex_NUMBER, FORMAT("#{}", i).data(), _selectedScanner, i);

                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", item._expandedEquation.data());
                    ImGui::EndTooltip();
                }
                if (_selectedScanner == i) {
                    if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
                        _deleteScannerID = i;
                    }
                }
                NextCell(ScannerSavedColumnIndex_NAME, item._name.data());
                ImGui::TableSetColumnIndex(ScannerSavedColumnIndex_OPERATIONS);

                if (ImGui::Button(FORMAT("{} {}##Operations", item._applied ? ICON_MD_STOP : ICON_MD_PLAY_ARROW, item._applied ? "Stop" : "Apply").data(), ImVec2(-FLT_MIN, 0))) {
                    if (not item._applied) {
                        if (not _strategyList.empty()) {
                            ScannerResultOutputT scannerResultOutput{
                                ._portfolio         = std::static_pointer_cast<Portfolio>(shared_from_this()),
                                ._parameterInfoList = _strategyList.front()->_parameterInfoList,
                            };
                            GlobalPortfolioScannerContainer.insert_or_assign(item._uniqueID, scannerResultOutput);
                        }
#ifdef TURNOFF_SCANNER
#else
                        item._applied = Scanner::GetInstance().EvaluateExp(item._uniqueID, item._expandedEquation, false);
#endif
                    } else {
#ifdef TURNOFF_SCANNER
#else
                        Scanner::GetInstance().ScannerUnsubscribe(item._uniqueID);
                        item._applied = false;
#endif
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
    for (const auto& item : _scannerFunctionListContainer) {
        if (item._selected) {
            ss << FORMAT("var {} := {}(token_);", item._variable, item._name, item._variable);
        }
    }
    int64_t uniqueID = time(nullptr);
    ss << FORMAT("var output := if(({}), ScannerAPI({}, {}, {}, token_), 0);\n", _equations, uniqueID, _strategyID, _selectedParam);
    ss << "output";

    _unfoldedFormula = ss.str();

    _scannerSaveContainer.emplace_back(SaveScannerItemT{._applied = false, ._uniqueID = uniqueID, ._name = _formulaName, ._expandedEquation = _unfoldedFormula});
}

void PortfolioScanner::Export(const std::string& path_) {
    if (_scannerSaveContainer.empty()) {
        std::remove(path_.data());
        return;
    }

    nlohmann::ordered_json root;
    for (const auto& scannerItem : _scannerSaveContainer) {
        nlohmann::json item;
        item["ID"]               = scannerItem._uniqueID;
        item["Name"]             = scannerItem._name;
        item["ExpandedEquation"] = scannerItem._expandedEquation;
        LOG(INFO, "Exporting Scanner {} {}", scannerItem._uniqueID, scannerItem._name)
        root.emplace_back(item);
    }
    std::fstream file(path_, std::ios::trunc | std::ios::out);
    if (not file.is_open()) {
        return;
    }
    file << root.dump();
    file.close();
}
void PortfolioScanner::Import(const std::string& path_) {
    std::fstream file(path_, std::ios::in);
    if (not file.is_open()) {
        return;
    }

    nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);
    std::ranges::for_each(root.items(), [&](const auto& valueType_) {
        auto        value            = valueType_.value();
        int64_t     uniqueID         = value.at("ID").template get<int64_t>();
        std::string name             = value.at("Name").template get<std::string>();
        std::string expandedEquation = value.at("ExpandedEquation").template get<std::string>();
        LOG(INFO, "Importing Scanner {} {}", uniqueID, name)
        _scannerSaveContainer.emplace_back(SaveScannerItemT{._applied = false, ._uniqueID = uniqueID, ._name = name, ._expandedEquation = expandedEquation});
    });
    LOG(INFO, "{} {}", __FUNCTION__, "Done")
    file.close();
}
