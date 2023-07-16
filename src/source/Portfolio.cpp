#include "../include/Portfolio.hpp"

#include <boost/algorithm/string.hpp>

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/ConfigLoader.hpp"
#include "../include/Configuration.hpp"
#include "../include/Enums.hpp"
#include "../include/MarketWatch.hpp"
#include "../include/Structure.hpp"
#include "../include/TableColumnInfo.hpp"
#include "../include/Utils.hpp"

extern AllContractT	   AllContract;
extern std::string	   StatusDisplay;
extern ClientCodeListT ClientCodeList;

#define ADDITIONAL_OPTION "Additional Options"

AddContractToMarketWatchSignal Portfolio::_addContractToMarketWatchSignal;

Portfolio::Portfolio(const std::string& workspaceName_, const std::string& strategyName_, boost::asio::io_context::strand& strand_)
    : PortfolioInterface(workspaceName_ + "[" + strategyName_ + "]", strategyName_, strand_) {
    _action = ExportImport_NONE;
}

void Portfolio::paint() {
    if (_toBeDeleted != -1 and _multipleSelectionCount > 0) {
        if (_multipleSelectionCount > 1) {
            RemoveSelection();
            _multipleSelectionCount = 0;
        } else {
            auto iterator = _strategyList.erase(_strategyList.begin() + _toBeDeleted);

            if (iterator != _strategyList.end()) {
                StrategyRowPtrT row = *iterator;
                row->Selected		= true;
            } else if (not _strategyList.empty()) {
                --iterator;
                StrategyRowPtrT row = *iterator;
                row->Selected		= true;
            }
        }

        Utils::RemovePortfolio();
        _toBeDeleted = -1;
    }
    if (ImGui::BeginTabItem(_name.data(), &_open, PortfolioTabFlags)) {
        DrawPortfolioWindow();
        ImGui::EndTabItem();
    }
    if (not _open) {
        _status = checkAnyActive();
        _open	= _status.Close;
        ImGui::OpenPopup(("Error Closing:- " + _name).data());
    }

    if (ImGui::BeginPopupModal(("Error Closing:- " + _name).data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Total Portfolio status:-");
        ImGui::LabelText("InActive", "[%d]", _status.Inactive);
        ImGui::LabelText("Active", "[%d]", _status.Active);
        ImGui::LabelText("Apply", "[%d]", _status.Apply);
        ImGui::LabelText("Waiting", "[%d]", _status.Waiting);
        ImGui::LabelText("Terminate", "[%d]", _status.Terminate);

        if (ImGui::Button(ICON_MD_ARROW_BACK " Understand", ImVec2(-1, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
    _scannerAddQueue.consume_one([&](const StrategyRowPtrT& row_) { _strategyList.push_back(row_); });
}

std::string Portfolio::getStrategyName() const {
    return _strategyName;
}

bool Portfolio::closed() const {
    return not _open;
}

void Portfolio::DrawPortfolioWindow() {
    ImGui::BeginDisabled(_portFolioNumber > MAX_PORTFOLIO_ALLOWED);
    if (ImGui::Button(ICON_MD_ADD_CIRCLE " New")) {
        _showGlobalParameter = true;
        ImGui::OpenPopup(("New Strategy:- " + _name).data());
    }
    ImGui::EndDisabled();
    if (_portFolioNumber < MAX_PORTFOLIO_ALLOWED) DrawNewStrategyPopUpWindow();

    ImGui::SameLine();
    if (ImGui::Button(FORMAT("{} Subscribe {} ##Subcribe", ICON_MD_PLAYLIST_PLAY, _multipleSelectionCount > 1 ? "Selected" : "All").data())) {
        _multipleSelectionCount > 1 ? subscribeSelected() : subscribeAll();
    }

    ImGui::SameLine();
    if (ImGui::Button(FORMAT("{} Apply {} ##Apply", ICON_MD_PLAY_ARROW, _multipleSelectionCount > 1 ? "Selected" : "All").data())) {
        _multipleSelectionCount > 1 ? applySelected() : applyAll();
    }

    ImGui::SameLine();
    if (ImGui::Button(FORMAT("{} Unsubscribe {} ##Unsubscribe", ICON_MD_STOP, _multipleSelectionCount > 1 ? "Selected" : "All").data())) {
        _multipleSelectionCount > 1 ? unsubscribeSelected() : unsubscribeAll();
    }

    ImGui::SameLine();
    if (ImGui::Button(ICON_MD_CLEAR_ALL " Clear")) {
        _strategyList.clear();
    }

    ImGui::SameLine();
    if (ImGui::Button(ICON_MD_TUNE " Global Param")) {
        ImGui::OpenPopup(("Global Params:- " + _name).data());
        _showGlobalParameter = true;
    }

    if (_showGlobalParameter) DrawGlobalParamPopupWindow();

    ImGui::SameLine();

    if (ImGui::Button(ICON_MD_MANAGE_SEARCH " Scanner")) {
        _showScanner = true;
        ImGui::OpenPopup(SCANNER_WINDOW);
    }
    if (_showScanner) {
        PortfolioScanner::paint(&_showScanner);
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_MD_APP_SETTINGS_ALT " Options")) ImGui::OpenPopup(ADDITIONAL_OPTION);
    if (ImGui::BeginPopup(ADDITIONAL_OPTION)) {
        ImVec2 buttonSize = ImVec2(120, 0);
        if (ImGui::Button(ICON_MD_REFRESH " Refresh", buttonSize)) {
            const std::string jsonData = ConfigLoader::Instance().getStrategyColumn(_strategyName);
            ParseConfig(jsonData);
        }
        ImGui::BeginDisabled(_exportActivated);
        if (ImGui::Button(ICON_MD_UPLOAD " Export", buttonSize)) {
            ImGuiFileDialog::Instance()->OpenDialog("FileManager", "File Manager", ".json", ".");
            _action = ExportImport_EXPORT;
        }
        ImGui::EndDisabled();

        if (ImGui::Button(ICON_MD_DOWNLOAD " Import", buttonSize)) {
            ImGuiFileDialog::Instance()->OpenDialog("FileManager", "File Manager", ".json", ".");
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
            _clipper.Begin(_strategyList.size());
            while (_clipper.Step()) {
                auto begin = _strategyList.begin() + _clipper.DisplayStart;
                auto end   = begin + (_clipper.DisplayEnd - _clipper.DisplayStart);
                int	 index = _clipper.DisplayStart;
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
        ParameterValueT&   info = value.second.Parameter;
        const std::string& name = value.first;
        ImGui::PushID(name.data());
        switch (value.second.Type) {
            case DataType_CLIENT: {
                if (ImGui::BeginCombo(name.data(), info.Text.data())) {
                    for (const auto& [exchangeName, client] : ClientCodeList) {
                        if (ImGui::Selectable(FORMAT("[{}] {}", Lancelot::toString(exchangeName), client).data())) {
                            info.Text = client;
                        }
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case DataType_CONTRACT: {
                if (value.second.SearchEnable) {
                    value.second.Filter.Draw(name.data());
                    if (value.second.Filter.IsActive()) {
                        ImGui::SameLine();
                        Utils::ContractFilter(value.second.Filter, info.Text);
                    }
                } else {
                    if (ImGui::BeginCombo(name.data(), info.Text.data())) {
                        _contractClipper.Begin(AllContract.size());
                        while (_contractClipper.Step()) {
                            auto begin = AllContract.begin() + _contractClipper.DisplayStart;
                            auto end   = begin + (_contractClipper.DisplayEnd - _contractClipper.DisplayStart);
                            for (auto iterator = begin; iterator < end; ++iterator) {
                                if (ImGui::Selectable(iterator->data())) {
                                    info.Text = *iterator;
                                }
                            }
                        }

                        ImGui::EndCombo();
                    }
                }
                ImGui::SameLine();
                ImGui::Checkbox("##Seach", &value.second.SearchEnable);
                break;
            }
            case DataType_INT: {
                ImGui::InputInt(name.data(), &info.Integer, 1);
                break;
            }
            case DataType_FLOAT: {
                ImGui::InputFloat(name.data(), &info.Floating, 0.01);
                break;
            }
            case DataType_TEXT: {
                ImGui::InputText(name.data(), &info.Text);
                break;
            }
            case DataType_RADIO: {
                ImGui::Checkbox(name.data(), &info.Check);
                break;
            }
            case DataType_COMBO: {
                std::string data(info.Text);
                std::replace(data.begin(), data.end(), ';', '\0');
                data.append("\0\0");
                ImGui::Combo(name.data(), &info.Integer, data.data());
                break;
            }
            case DataType_UPDATES:
            case DataType_END: break;
        }
        ImGui::PopID();
    }
}

void Portfolio::DrawStrategyRow(StrategyRowPtrT& row_, int index_) {
    bool addToMarketWatch = false;
    ImGui::PushID(row_->PF);

    if (FirstCell(0, FORMAT("{}", row_->PF).data(), row_->Selected, true)) {
        if (not ImGui::GetIO().KeyCtrl) {
            ResetSelection();
            _multipleSelectionCount = 0;
        }
        row_->Selected ^= 1;
        _multipleSelectionCount += row_->Selected ? 1 : -1;
    }
    if (row_->Selected) {
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
    const ImVec4 color = GetStatusColor(row_->Status, row_->Changed);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::BeginDisabled(row_->Status == StrategyStatus_PENDING);
    if (ImGui::Checkbox(fmt::format("{}##SubscribedCheckBok", StrategyStatusType[row_->Status]).data(), &row_->Subscribed)) {
        doStrategyAction(row_, _strategyName, row_->Subscribed ? Lancelot::RequestType_SUBSCRIBE : Lancelot::RequestType_UNSUBSCRIBE);
    }
    ImGui::EndDisabled();
    ImGui::PopStyleColor();
    ImGui::TableSetColumnIndex(2);
    ImGui::BeginDisabled(not row_->Subscribed or row_->Status == StrategyStatus_PENDING);
    if (ImGui::Button("Apply##ApplyButton", ImVec2(-FLT_MIN, 0.0f))) {
        doStrategyAction(row_, _strategyName, Lancelot::RequestType_APPLY);
    }
    ImGui::EndDisabled();

    int column = 3;
    for (ParameterInfoListT::value_type& value : row_->ParameterInfoList) {
        if (ImGui::TableSetColumnIndex(column)) {
            ParameterValueT&  info = value.second.Parameter;
            const std::string name = "##" + value.first;
            ImGui::PushItemWidth(-FLT_MIN);

            switch (value.second.Type) {
                case DataType_COMBO:
                case DataType_CLIENT:
                case DataType_UPDATES: {
                    ImGui::Text("%s", info.Text.data());
                    break;
                }
                case DataType_CONTRACT: {
                    ImGui::Text("%s", info.Text.data());
                    if (ImGui::IsItemHovered()) {
                        MarketWatch::ToolTipDisplay(value.second.Self);
                    }
                    if (addToMarketWatch) {
                        _addContractToMarketWatchSignal(info.Text);
                    }
                    break;
                }
                case DataType_INT: {
                    if (row_->Selected) {
                        if (ImGui::InputInt(name.data(), &info.Integer, 1, 100)) {
                            row_->Changed = true;
                        }
                    } else {
                        ImGui::Text("%d", info.Integer);
                    }
                    break;
                }
                case DataType_FLOAT: {
                    if (row_->Selected) {
                        if (ImGui::InputFloat(name.data(), &info.Floating, 0.01, 1)) {
                            row_->Changed = true;
                        }
                    } else {
                        ImGui::Text("%.2f", info.Floating);
                    }
                    break;
                }
                case DataType_TEXT: {
                    if (row_->Selected) {
                        if (ImGui::InputText(name.data(), &info.Text)) {
                            row_->Changed = true;
                        }
                    } else {
                        ImGui::Text("%s", info.Text.data());
                    }
                    break;
                }
                case DataType_RADIO: {
                    if (row_->Selected) {
                        if (ImGui::Checkbox(name.data(), &info.Check)) {
                            row_->Changed = true;
                        }
                    } else {
                        ImGui::Text("%b", info.Check);
                    }
                    break;
                }
                case DataType_END: break;
            }
            ImGui::PopItemWidth();
        }
        column += 1;
    }
    ImGui::PopID();
}

void Portfolio::DrawGlobalParam() {
    for (GlobalParameterInfoT& value : _globalParamList) {
        const std::string name = value.Name;
        ImGui::PushID(name.data());

        switch (value.Info.Type) {
            case DataType_CLIENT: {
                if (ImGui::BeginCombo(name.data(), value.Info.Parameter.Text.data())) {
                    for (const auto& [exchangeName, client] : ClientCodeList) {
                        if (ImGui::Selectable(FORMAT("[{}] {}", Lancelot::toString(exchangeName), client).data())) {
                            value.Info.Parameter.Text = client;
                        }
                    }
                    ImGui::EndCombo();
                }
                break;
            }
            case DataType_INT: {
                ImGui::Checkbox("##Update", &value.Update);
                ImGui::SameLine();
                ImGui::InputInt(name.data(), &value.Info.Parameter.Integer, 1, 100);
                break;
            }
            case DataType_FLOAT: {
                ImGui::Checkbox("##Update", &value.Update);
                ImGui::SameLine();
                ImGui::InputFloat(name.data(), &value.Info.Parameter.Floating, 0.01, 1);
                break;
            }
            case DataType_TEXT: {
                ImGui::Checkbox("##Update", &value.Update);
                ImGui::SameLine();
                ImGui::InputText(name.data(), &value.Info.Parameter.Text);
                break;
            }
            case DataType_RADIO: {
                ImGui::Checkbox("##Update", &value.Update);
                ImGui::SameLine();
                ImGui::Checkbox(name.data(), &value.Info.Parameter.Check);
                break;
            }
            case DataType_COMBO: {
                ImGui::Checkbox("##Update", &value.Update);
                ImGui::SameLine();

                std::string data(value.Info.Parameter.Text);
                std::replace(data.begin(), data.end(), ';', '\0');
                data.append("\0\0");
                ImGui::Combo(name.data(), &value.Info.Parameter.Integer, data.data());
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
    row->PF				= ++_portFolioNumber;
    row->Subscribed		= false;
    row->Selected		= false;
    row->Status			= StrategyStatus_INACTIVE;

    for (ParameterInfoListT::value_type valueType : _paramList) {
        ParameterInfoT& info = valueType.second;
        if (info.Type == DataType_CONTRACT) {
            info.Self = ContractInfo::GetLiveDataRef(Lancelot::ContractInfo::GetToken(info.Parameter.Text));
        } else if (info.Type == DataType_COMBO) {
            std::string				 options = info.Parameter.Text;
            std::vector<std::string> result;
            boost::split(result, options, boost::is_any_of(";"));
            info.Parameter.Text = result.at(info.Parameter.Integer);
        }
        row->ParameterInfoList.emplace(valueType);
    }

    _strategyList.emplace_back(row);
    Utils::AppendPortfolio(row->PF, row);
}

void Portfolio::ModifyGlobalParam() {
    for (GlobalParameterInfoT& info : _globalParamList) {
        if (info.Update) {
            updateAll(info);
        }
    }
}

std::string Portfolio::getName() const {
    return _name;
}

void		Portfolio::ResetSelection() {
#pragma omp parallel
#pragma omp for
    for (const StrategyListT::value_type& valueType_ : _strategyList) {
        valueType_->Selected = false;
    }
}

void Portfolio::RemoveSelection() {
    std::erase_if(_strategyList, [](const StrategyListT::value_type& valueType_) {
        return (valueType_->Status == StrategyStatus_INACTIVE or valueType_->Status == StrategyStatus_TERMINATED) and valueType_->Selected;
    });
}

void Portfolio::DrawNewStrategyPopUpWindow() {
    if (ImGui::BeginPopupModal(("New Strategy:- " + _name).data(), &_showGlobalParameter, ImGuiWindowFlags_AlwaysAutoResize)) {
        DrawNewPortfolioCreation();

        if (ImGui::Button(ICON_MD_DONE " Submit")) {
            AppendStrategy();

            if (_portFolioNumber == MAX_PORTFOLIO_ALLOWED) {
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
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(("Global Params:- " + _name).data(), &_showGlobalParameter, ImGuiWindowFlags_AlwaysAutoResize)) {
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
void Portfolio::setCallback(const boost::signals2::slot<void(const std::string&), boost::function<void(const std::string&)>>& slot_) {
    _addContractToMarketWatchSignal.connect(slot_);
}

void Portfolio::AddScannerPortfolio(const ParameterInfoListT& list_) {
    StrategyRowPtrT row	   = std::make_shared<StrategyRowT>();
    row->PF				   = ++_portFolioNumber;
    row->Subscribed		   = false;
    row->Selected		   = false;
    row->Status			   = StrategyStatus_INACTIVE;
    row->ParameterInfoList = list_;
    _scannerAddQueue.push(row);
    Utils::AppendPortfolio(row->PF, row);
}
