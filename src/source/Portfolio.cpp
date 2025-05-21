#include "Portfolio.hpp"

#include "API/Common.hpp"
#include "API/ContractInfo.hpp"
#include "Arthur_Fwd.hpp"
#include "ConfigLoader.hpp"
#include "Configuration.hpp"
#include "Enums.hpp"
#include "ImGuiFileDialog.h"
#include "MarketWatch.hpp"
#include "Structure.hpp"
#include "TableColumnInfo.hpp"
#include "Utils.hpp"
#include "misc/cpp/imgui_stdlib.h"

#include <boost/algorithm/string.hpp>

#include <algorithm>

extern AllContractT    AllContract;
extern std::string     StatusDisplay;
extern ClientCodeListT ClientCodeList;

#define ADDITIONAL_OPTION     "Additional Options"
#define NEW_STRATEGY_CREATION "New Strategy"

Portfolio::Portfolio(const std::string& workspaceName_, const std::string& strategyName_, ExecutorT& strand_)
    : PortfolioInterface(workspaceName_ + "[" + strategyName_ + "]", strategyName_, strand_) {
    _action = ExportImport_NONE;
}

void Portfolio::Paint() {
    if (_toBeDeleted != -1 and _multipleSelectionCount > 0) {
        if (_multipleSelectionCount > 1) {
            RemoveSelection();
            _multipleSelectionCount = 0;
        } else {
            auto iterator = _strategyList.erase(_strategyList.begin() + _toBeDeleted);

            if (iterator != _strategyList.end()) {
                iterator->get()->_selected = true;
            } else if (not _strategyList.empty()) {
                --iterator;
                iterator->get()->_selected = true;
            }
        }

        Utils::RemovePortfolio();
        _toBeDeleted = -1;
    }
    if (ImGui::BeginTabItem(_name.data(), &_open, PortfolioTabFlags)) {
        DrawPortfolioWindow();
        ImGui::EndTabItem();
    }
    if (Closed()) {
        _status = CheckAnyActive();
        _open   = _status._close;
        ImGui::OpenPopup(("Error Closing:- " + GetName()).data());
    }

    if (ImGui::BeginPopupModal(("Error Closing:- " + GetName()).data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Total Portfolio status:-");
        ImGui::LabelText("InActive", "[%ud]", _status._inactive);
        ImGui::LabelText("Active", "[%ud]", _status._active);
        ImGui::LabelText("Apply", "[%ud]", _status._apply);
        ImGui::LabelText("Waiting", "[%ud]", _status._waiting);
        ImGui::LabelText("Terminate", "[%ud]", _status._terminate);

        if (ImGui::Button(ICON_MD_ARROW_BACK " Understand", ImVec2(-1, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
    _scannerAddQueue.consume_all([&](StrategyRowPtrT row_) { _strategyList.push_back(std::move(row_)); });
}

auto Portfolio::Closed() const -> bool {
    return not IsOpen();
}

void Portfolio::DrawPortfolioWindow() {
    ImGui::BeginDisabled(GetPortfolio() > GetMaxPortfolio());
    if (ImGui::Button(ICON_MD_ADD_CIRCLE " New")) {
        ImGui::OpenPopup((NEW_STRATEGY_CREATION + GetStrategyName()).data());
    }

    ImGui::EndDisabled();
    DrawNewStrategyPopUpWindow();

    ImGui::SameLine();
    if (ImGui::Button(FORMAT("{} Subscribe {} ##Subcribe", ICON_MD_PLAYLIST_PLAY, _multipleSelectionCount > 1 ? "Selected" : "All").data())) {
        _multipleSelectionCount > 1 ? SubscribeSelected() : SubscribeAll();
    }

    ImGui::SameLine();
    if (ImGui::Button(FORMAT("{} Apply {} ##Apply", ICON_MD_PLAY_ARROW, _multipleSelectionCount > 1 ? "Selected" : "All").data())) {
        _multipleSelectionCount > 1 ? ApplySelected() : ApplyAll();
    }

    ImGui::SameLine();
    if (ImGui::Button(FORMAT("{} Unsubscribe {} ##Unsubscribe", ICON_MD_STOP, _multipleSelectionCount > 1 ? "Selected" : "All").data())) {
        _multipleSelectionCount > 1 ? UnsubscribeSelected() : UnsubscribeAll();
    }

    ImGui::SameLine();
    if (ImGui::Button(ICON_MD_CLEAR_ALL " Clear")) {
        _strategyList.clear();
    }

    ImGui::SameLine();
    if (ImGui::Button(ICON_MD_TUNE " Global Param")) {
        ImGui::OpenPopup(("Global Params:- " + _name).data());
    }

    DrawGlobalParamPopupWindow();

    ImGui::SameLine();

    if (ImGui::Button(ICON_MD_MANAGE_SEARCH " Scanner")) {
        _showScanner = true;
        ImGui::OpenPopup(SCANNER_WINDOW);
    }
    if (_showScanner) {
        PortfolioScanner::Paint(&_showScanner);
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_MD_APP_SETTINGS_ALT " Options")) ImGui::OpenPopup(ADDITIONAL_OPTION);
    if (ImGui::BeginPopup(ADDITIONAL_OPTION)) {
        ImVec2 buttonSize = ImVec2(120, 0);
        if (ImGui::Button(ICON_MD_REFRESH " Refresh", buttonSize)) {
            const std::string jsonData = ConfigLoader::Instance().GetStrategyColumn(_strategyName);
            ParseConfig(jsonData);
        }
        ImGui::BeginDisabled(_exportActivated);
        if (ImGui::Button(ICON_MD_UPLOAD " Export", buttonSize)) {
            ImGuiFileDialog::Instance()->OpenDialog("FileManager", "File Manager", ".json");
            _action = ExportImport_EXPORT;
        }
        ImGui::EndDisabled();

        if (ImGui::Button(ICON_MD_DOWNLOAD " Import", buttonSize)) {
            ImGuiFileDialog::Instance()->OpenDialog("FileManager", "File Manager", ".json");
            _action = ExportImport_IMPORT;
        }

        DrawFileManagerWindow();

        ImGui::EndPopup();
    }
    auto ColumnFlags = TableColumnFlags;
    ImGui::SameLine();
    ImGui::CheckboxFlags("Resize Column", &ColumnFlags, ImGuiTableColumnFlags_WidthStretch);

    ImGui::Separator();
    const float frameHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild(("Separate space " + _name).data(), ImVec2(-FLT_MIN, -frameHeight))) {
        if (ImGui::BeginTable(__FUNCTION__, _paramList.size() + 3, TableFlags)) {
            ImGui::TableSetupScrollFreeze(3, 0);
            ImGui::TableSetupColumn("PF", TableColumnFlags);
            ImGui::TableSetupColumn("Status", TableColumnFlags);
            ImGui::TableSetupColumn("Action", TableColumnFlags);

            for (const auto& [columnName, _] : _paramList) {
                ImGui::TableSetupColumn(columnName.data(), ColumnFlags);
            }
            ImGui::TableHeadersRow();
            _clipper.Begin(static_cast<int>(_strategyList.size()));
            while (_clipper.Step()) {
                auto begin = _strategyList.begin() + _clipper.DisplayStart;
                auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
                int  index = _clipper.DisplayStart;
                for (auto& iterator = begin; iterator != end; ++iterator) {
                    ImGui::TableNextRow();
                    DrawStrategyRow(*iterator, index);
                    index += 1;
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
    ImGui::Separator();
    ImGui::Text("| Total : [%zu] | Selected : [%d] |", _strategyList.size(), _multipleSelectionCount);
}

void Portfolio::DrawNewPortfolioCreation() {
    for (ParameterInfoListT::value_type& value : _paramList) {
        ParameterValueT&   info = value.second._parameter;
        const std::string& name = value.first;
        ImGui::PushID(name.data());
        switch (value.second._type) {
            case DataType_CLIENT: {
                if (ImGui::BeginCombo(name.data(), info._text.data())) {
                    for (const auto& [exchangeName, client] : ClientCodeList) {
                        if (ImGui::Selectable(FORMAT("[{}] {}", Lancelot::ToString(exchangeName), client).data())) {
                            info._text = client;
                        }
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case DataType_CONTRACT: {
                if (value.second._searchEnable) {
                    value.second._filter.Draw(name.data());
                    if (value.second._filter.IsActive()) {
                        ImGui::SameLine();
                        Utils::ContractFilter(value.second._filter, info._text);
                    }
                } else {
                    if (ImGui::BeginCombo(name.data(), info._text.data())) {
                        _contractClipper.Begin(static_cast<int>(AllContract.size()));
                        while (_contractClipper.Step()) {
                            auto begin = AllContract.begin() + _contractClipper.DisplayStart;
                            auto end   = begin + (_contractClipper.DisplayEnd - _contractClipper.DisplayStart);
                            for (auto iterator = begin; iterator < end; ++iterator) {
                                if (ImGui::Selectable(iterator->data())) {
                                    info._text = *iterator;
                                }
                            }
                        }

                        ImGui::EndCombo();
                    }
                }
                ImGui::SameLine();
                ImGui::Checkbox("##Seach", &value.second._searchEnable);
                break;
            }
            case DataType_INT: {
                ImGui::InputInt(name.data(), &info._integer, 1);
                break;
            }
            case DataType_FLOAT: {
                ImGui::InputFloat(name.data(), &info._floating, 0.01F);
                break;
            }
            case DataType_TEXT: {
                ImGui::InputText(name.data(), &info._text);
                break;
            }
            case DataType_RADIO: {
                ImGui::Checkbox(name.data(), &info._check);
                break;
            }
            case DataType_COMBO: {
                std::string data(info._text);
                std::ranges::replace(data, ';', '\0');
                data.append("\0\0");
                ImGui::Combo(name.data(), &info._integer, data.data());
                break;
            }
            case DataType_UPDATES:
            case DataType_END:
                break;
        }
        ImGui::PopID();
    }
}

void Portfolio::DrawStrategyRow(StrategyRowPtrT& row_, int index_) {
    bool addToMarketWatch = false;
    ImGui::PushID(row_->_portfolio);

    if (FirstCell(0, FORMAT("{}", row_->_portfolio).data(), row_->_selected, true)) {
        if (not ImGui::GetIO().KeyCtrl) {
            ResetSelection();
            _multipleSelectionCount = 0;
        }
        row_->_selected ^= 1;
        _multipleSelectionCount += row_->_selected ? 1 : -1;
    }
    if (row_->_selected and (row_->_status == StrategyStatus_TERMINATED or row_->_status == StrategyStatus_INACTIVE)) {
        if (ImGui::IsKeyPressed(ImGuiKey_Delete)) {
            _toBeDeleted = index_;
        }
        if (ImGui::BeginPopupContextItem("Context Menu", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::Selectable(ICON_MD_ADD_BOX " Add to Market Watch")) {
                addToMarketWatch = true;
            }

            ImGui::EndPopup();
        }
    }

    ImGui::TableSetColumnIndex(1);
    const ImVec4 color = GetStatusColor(row_->_status, row_->_changed);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::BeginDisabled(row_->_status == StrategyStatus_PENDING);
    if (ImGui::Checkbox(FORMAT("{}##SubscribedCheckBok", StrategyStatusType[row_->_status]).data(), &row_->_subscribed)) {
        DoStrategyAction(row_, _strategyName, row_->_subscribed ? Lancelot::RequestType_SUBSCRIBE : Lancelot::RequestType_UNSUBSCRIBE);
    }
    ImGui::EndDisabled();
    ImGui::PopStyleColor();
    ImGui::TableSetColumnIndex(2);
    ImGui::BeginDisabled(not row_->_subscribed or row_->_status == StrategyStatus_PENDING);
    if (ImGui::Button("Apply##ApplyButton", ImVec2(-FLT_MIN, 0.0f))) {
        DoStrategyAction(row_, _strategyName, Lancelot::RequestType_APPLY);
    }
    ImGui::EndDisabled();

    int column = 3;
    for (ParameterInfoListT::value_type& value : row_->_parameterInfoList) {
        if (ImGui::TableSetColumnIndex(column)) {
            ParameterValueT&  info = value.second._parameter;
            const std::string name = "##" + value.first;
            ImGui::PushItemWidth(-FLT_MIN);

            switch (value.second._type) {
                case DataType_COMBO:
                case DataType_CLIENT:
                case DataType_UPDATES: {
                    ImGui::Text("%s", info._text.data());
                    break;
                }
                case DataType_CONTRACT: {
                    ImGui::Text("%s", info._text.data());
                    if (ImGui::IsItemHovered()) {
                        MarketWatch::ToolTipDisplay(value.second._marketWatch);
                    }
                    if (addToMarketWatch) {
                        AddContractFunction(info._text);
                    }
                    break;
                }
                case DataType_INT: {
                    if (row_->_selected) {
                        if (ImGui::InputInt(name.data(), &info._integer, 1, 100)) {
                            row_->_changed = true;
                        }
                    } else {
                        ImGui::Text("%d", info._integer);
                    }
                    break;
                }
                case DataType_FLOAT: {
                    if (row_->_selected) {
                        if (ImGui::InputFloat(name.data(), &info._floating, 0.01F, 1)) {
                            row_->_changed = true;
                        }
                    } else {
                        ImGui::Text("%.2f", info._floating);
                    }
                    break;
                }
                case DataType_TEXT: {
                    if (row_->_selected) {
                        if (ImGui::InputText(name.data(), &info._text)) {
                            row_->_changed = true;
                        }
                    } else {
                        ImGui::Text("%s", info._text.data());
                    }
                    break;
                }
                case DataType_RADIO: {
                    if (row_->_selected) {
                        if (ImGui::Checkbox(name.data(), &info._check)) {
                            row_->_changed = true;
                        }
                    } else {
                        ImGui::Text("%d", info._check);
                    }
                    break;
                }
                case DataType_END:
                    break;
            }
            ImGui::PopItemWidth();
        }
        column += 1;
    }
    ImGui::PopID();
}

void Portfolio::DrawGlobalParam() {
    for (GlobalParameterInfoT& value : _globalParamList) {
        const std::string name = value._name;
        ImGui::PushID(name.data());

        switch (value._parameterInfo._type) {
            case DataType_CLIENT: {
                if (ImGui::BeginCombo(name.data(), value._parameterInfo._parameter._text.data())) {
                    for (const auto& [exchangeName, client] : ClientCodeList) {
                        if (ImGui::Selectable(FORMAT("[{}] {}", Lancelot::ToString(exchangeName), client).data())) {
                            value._parameterInfo._parameter._text = client;
                        }
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case DataType_INT: {
                ImGui::Checkbox("##Update", &value._update);
                ImGui::SameLine();
                ImGui::InputInt(name.data(), &value._parameterInfo._parameter._integer, 1, 100);
                break;
            }
            case DataType_FLOAT: {
                ImGui::Checkbox("##Update", &value._update);
                ImGui::SameLine();
                ImGui::InputFloat(name.data(), &value._parameterInfo._parameter._floating, 0.01F, 1);
                break;
            }
            case DataType_TEXT: {
                ImGui::Checkbox("##Update", &value._update);
                ImGui::SameLine();
                ImGui::InputText(name.data(), &value._parameterInfo._parameter._text);
                break;
            }
            case DataType_RADIO: {
                ImGui::Checkbox("##Update", &value._update);
                ImGui::SameLine();
                ImGui::Checkbox(name.data(), &value._parameterInfo._parameter._check);
                break;
            }
            case DataType_COMBO: {
                ImGui::Checkbox("##Update", &value._update);
                ImGui::SameLine();

                std::string data(value._parameterInfo._parameter._text);
                std::replace(data.begin(), data.end(), ';', '\0');
                data.append("\0\0");
                ImGui::Combo(name.data(), &value._parameterInfo._parameter._integer, data.data());
                break;
            }
            case DataType_UPDATES:
            case DataType_CONTRACT:
            case DataType_END: {
                break;
            }
        }
        ImGui::PopID();
    }
}

void Portfolio::AppendStrategy() {
    StrategyRowPtrT row = std::make_shared<StrategyRowT>();
    row->_portfolio     = ++PortFolioNumber;
    row->_subscribed    = false;
    row->_selected      = false;
    row->_status        = StrategyStatus_INACTIVE;

    for (ParameterInfoListT::value_type valueType : _paramList) {
        ParameterInfoT& info = valueType.second;
        if (info._type == DataType_CONTRACT) {
            info._marketWatch = ContractInfo::GetLiveDataRef(Lancelot::ContractInfo::GetToken(info._parameter._text));
        } else if (info._type == DataType_COMBO) {
            std::string              options = info._parameter._text;
            std::vector<std::string> result;
            boost::split(result, options, boost::is_any_of(";"));
            info._parameter._text = result.at(static_cast<size_t>(info._parameter._integer));
        }
        row->_parameterInfoList.emplace(valueType);
    }

    _strategyList.emplace_back(row);
    Utils::AppendPortfolio(row->_portfolio, row);
}

void Portfolio::ModifyGlobalParam() {
    for (GlobalParameterInfoT& info : _globalParamList) {
        if (info._update) {
            UpdateAll(info);
        }
    }
}

void Portfolio::ResetSelection() {
#pragma omp parallel
#pragma omp for
    for (const StrategyListT::value_type& valueType : _strategyList) {
        valueType->_selected = false;
    }
}

void Portfolio::RemoveSelection() {
    std::erase_if(_strategyList, [](const StrategyListT::value_type& valueType_) {
        return (valueType_->_status == StrategyStatus_INACTIVE or valueType_->_status == StrategyStatus_TERMINATED) and valueType_->_selected;
    });
}

void Portfolio::DrawNewStrategyPopUpWindow() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    if (ImGui::BeginPopupModal((NEW_STRATEGY_CREATION + _strategyName).data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        DrawNewPortfolioCreation();

        if (ImGui::Button(ICON_MD_DONE " Submit")) {
            AppendStrategy();

            if (PortFolioNumber == MaxPortfolioAllowed) {
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SetItemDefaultFocus();

        ImGui::SameLine();
        if (ImGui::Button(ICON_MD_CANCEL " Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Portfolio::DrawGlobalParamPopupWindow() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    if (ImGui::BeginPopupModal(("Global Params:- " + _name).data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        DrawGlobalParam();
        if (ImGui::Button(ICON_MD_UPDATE " Update")) {
            ModifyGlobalParam();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button(ICON_MD_CANCEL " Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Portfolio::DrawFileManagerWindow() {
    if (ImGuiFileDialog::Instance()->Display("FileManager")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            if (_action == ExportImport_EXPORT) {
                _future = std::async(std::launch::async, [this, path = std::forward<std::string>(filePathName)]() { Exports(path); });
            } else {
                Imports(filePathName);
            }
            _action = ExportImport_NONE;
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void Portfolio::AddScannerPortfolio(const ParameterInfoListT& list_) {
    StrategyRowPtrT row     = std::make_shared<StrategyRowT>();
    row->_portfolio         = ++PortFolioNumber;
    row->_subscribed        = false;
    row->_selected          = false;
    row->_status            = StrategyStatus_INACTIVE;
    row->_parameterInfoList = list_;
    _scannerAddQueue.push(row);
    Utils::AppendPortfolio(row->_portfolio, row);
}
