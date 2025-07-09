//
// Created by VIKLOD on 25-01-2023.
//

#include "TemplateBuilder.hpp"

#include "ConfigLoader.hpp"
#include "Configuration.hpp"
#include "Enums.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"
#include "misc/cpp/imgui_stdlib.h"

#include <nlohmann/json.hpp>

extern StrategyNameListT StrategyNameList;
extern std::string       StatusDisplay;

TemplateBuilder::TemplateBuilder(bool& show_) : _show(show_) {}

void TemplateBuilder::Paint(bool* show_) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    ImGui::SetNextWindowSize(ImVec2{ImGui::GetMainViewport()->Size.x / 2, ImGui::GetMainViewport()->Size.y / 2}, ImGuiCond_FirstUseEver);
    if (not ImGui::BeginPopupModal(BeginColumnGenerator, show_)) {
        return;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::PushItemWidth(ImGui::GetWindowWidth() / 3.0f);

    ImGui::InputTextWithHint("Strategy Name", "Enter Strategy Name", &_strategyName);
    ImGui::SameLine();

    if (ImGui::BeginCombo("Load Column", _strategyLoad.data())) {
        for (const auto& item : StrategyNameList) {
            if (ImGui::Selectable(item.data(), item == _strategyLoad)) {
                _strategyLoad = item;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    if (ImGui::Button(ICON_MD_DOWNLOAD " Load", ImVec2(-FLT_MIN, 0.0f))) {
        ParseConfig(ConfigLoader::Instance().GetStrategyColumn(_strategyLoad));
        _strategyName.clear();
        _strategyName.clear();
    }

    ImGui::Separator();

    ImGui::InputText("Parameter Name", &_parameterName);
    ImGui::SameLine();

    if (ImGui::Combo("Type", &_parameterType, ColumnGeneratorDataType, DataType_END)) {
        SetDefaultValue(static_cast<DataType>(_parameterType));
    }

    ImGui::InputText("Parameter Value", &_parameterValue);
    ImGui::SameLine();

    ImGui::BeginDisabled(_parameterName.empty());
    if (ImGui::Button(ICON_MD_ADD_BOX " Add Parameter")) {
        AppendNewParameter();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(_selectedRow.empty());
    if (ImGui::Button(ICON_MD_DELETE " Remove Parameter")) {
        StatusDisplay = FORMAT("Strategy ({}) :- Parameter Removed [ Name : ({}) ]", _strategyName, _selectedRow);
        _parameterList.erase(_selectedRow);
        _selectedRow.clear();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(_strategyName.empty());
    if (ImGui::Button(ICON_MD_SAVE " Save Config")) {
        std::string config = GetConfig();
        StatusDisplay      = FORMAT("Parameter updated : StrategyName ({}) -> Config : ({})", _strategyName, config);
        ConfigLoader::Instance().SaveStrategyColumn(_strategyName, config);
    }
    ImGui::EndDisabled();

    ImGui::PopItemWidth();
    ImGui::Separator();

    DrawTable();
    ImGui::EndPopup();
}

std::string TemplateBuilder::GetConfig() {
    nlohmann::ordered_json parameter;
    for (const ContainerT::value_type& value_type : _parameterList) {
        const std::string& name = value_type.first;
        const ColumnInfoT& info = value_type.second;

        nlohmann::ordered_json property;
        property["Value"]    = info._value;
        property["DataType"] = info._type;
        parameter[name]      = property;
    }
    nlohmann::ordered_json json;
    json["Name"]   = std::string(_strategyName);
    json["Params"] = parameter;
    return json.dump();
}

void TemplateBuilder::ParseConfig(std::string_view config_) {
    nlohmann::ordered_json json        = nlohmann::ordered_json::parse(config_);
    auto                   paramConfig = json["Params"];
    _parameterList.clear();
    for (const auto& item : paramConfig.items()) {
        const auto& value = item.value();
        ColumnInfoT param{._type = static_cast<DataType>(value["DataType"].get<int>()), ._value = value["Value"].get<std::string>()};
        _parameterList.emplace(item.key(), param);
    }
}

void TemplateBuilder::DrawTable() {
    if (ImGui::BeginTable("Parameter Table", ColumnGeneratorColumnIndex_END, TableFlags)) {
        ImGui::TableSetupScrollFreeze(0, 1);  // Make top row always visible
        for (const auto& columnName : ColumnGeneratorTableName) {
            ImGui::TableSetupColumn(columnName, TableColumnFlags | ImGuiTableColumnFlags_WidthStretch);
        }
        ImGui::TableHeadersRow();
        for (ContainerT::value_type& value_type : _parameterList) {
            const std::string& name = value_type.first;
            ColumnInfoT&       info = value_type.second;

            ImGui::TableNextRow();

            if (FirstCell(ColumnGeneratorColumnIndex_NAME, name.data(), _selectedRow, name)) {
                _selectedRow    = name;
                _parameterName  = name;
                _parameterValue = info._value;
                _parameterType  = info._type;
            }

            NextCell(ColumnGeneratorColumnIndex_TYPE, ColumnGeneratorDataType[info._type]);
            NextCell(ColumnGeneratorColumnIndex_VALUE, info._value.data());
        }
        ImGui::EndTable();
    }
}

void TemplateBuilder::AppendNewParameter() {
    ColumnInfoT info{._type = static_cast<DataType>(_parameterType), ._value = _parameterValue};
    _parameterList.insert_or_assign(_parameterName, info);
    StatusDisplay = FORMAT("Strategy ({}) :- Parameter Added [ Name : ({}), Value: ({}) ]", _strategyName, _parameterName, _parameterValue);

    _parameterName.clear();
    _parameterValue.clear();
}
void TemplateBuilder::SetDefaultValue(DataType dataType_) {
    switch (dataType_) {
        case DataType_CLIENT: {
            _parameterValue = "PRO";
            break;
        }
        case DataType_END:
        case DataType_UPDATES:
        case DataType_TEXT:
        case DataType_COMBO:
        case DataType_CONTRACT: {
            break;
        }
        case DataType_FLOAT:
        case DataType_INT:
        case DataType_RADIO: {
            _parameterValue = "0";
        }
    }
}
