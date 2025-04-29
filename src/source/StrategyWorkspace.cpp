#include "StrategyWorkspace.hpp"

#include "Configuration.hpp"
#include "Portfolio.hpp"
#include "Structure.hpp"

#include <imgui_stdlib.h>
#include <nlohmann/json.hpp>

#include <fstream>

extern StrategyNameListT StrategyNameList;

#define STRATEGY_CONFIG_FILE_NAME        "Config/StrategyWorkspace.json"
#define CREATE_NEW_WORKSPACE_WINDOW_NAME "Add New Workspace"
#define STRATEGY_CANVAS_NAME             "Workspace Canvas"

StrategyWorkspace::StrategyWorkspace(ExecutorT& strand_) : _strand(strand_) {
    Imports(STRATEGY_CONFIG_FILE_NAME);
}

void StrategyWorkspace::Paint(bool* show_) {
    if (*show_) {
        DrawWindow(show_);
    }
}

void StrategyWorkspace::DrawAddNewWorkspace() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    if (ImGui::BeginPopupModal(CREATE_NEW_WORKSPACE_WINDOW_NAME, &_newWorkspace, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputTextWithHint("Workspace Name", "Set Workspace Name", &_strategyWorkspaceName);
        if (ImGui::BeginCombo("Strategy List", _strategyListIndex.data())) {
            for (const auto& item : StrategyNameList) {
                if (ImGui::Selectable(item.data())) {
                    _strategyListIndex = item;
                }
            }
            ImGui::EndCombo();
        }

        ImGui::BeginDisabled(_strategyWorkspaceName.empty());
        if (ImGui::Button(ICON_MD_DONE " Submit")) {
            _portfolioContainer.emplace(_strategyWorkspaceName, std::make_shared<Portfolio>(_strategyWorkspaceName, _strategyListIndex, _strand));
            _strategyWorkspaceName.clear();
            Exports(STRATEGY_CONFIG_FILE_NAME);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndDisabled();
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button(ICON_MD_CANCEL " Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void StrategyWorkspace::Exports(const std::string& path_) {
    if (_portfolioContainer.empty()) {
        std::remove(path_.data());
        return;
    }

    nlohmann::ordered_json root;
    std::ranges::for_each(_portfolioContainer, [&](const PortfolioContainerT::value_type& valueType_) { root[valueType_.first.data()] = valueType_.second->GetStrategyName(); });

    std::fstream file(path_, std::ios::trunc | std::ios::out);
    if (not file.is_open()) {
        return;
    }
    file << root.dump();
    file.close();
}

void StrategyWorkspace::Imports(const std::string& path_) {
    std::fstream file(path_, std::ios::in);
    if (not file.is_open()) {
        return;
    }

    nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);
    std::ranges::for_each(root.items(), [&](const auto& valueType_) {
        const auto& key = valueType_.key();
        _portfolioContainer.emplace(key, std::make_shared<Portfolio>(key, valueType_.value(), _strand));
    });

    file.close();
}

void StrategyWorkspace::DrawWindow(bool* show_) {
    if (ImGui::Begin("Strategy Workspace", show_)) {
        if (ImGui::Button(ICON_MD_CREATE " Create Workspace")) {
            ImGui::OpenPopup(CREATE_NEW_WORKSPACE_WINDOW_NAME);
            _newWorkspace = true;
        }

        if (_newWorkspace) {
            DrawAddNewWorkspace();
        }
        ImGui::Separator();
        if (ImGui::BeginTabBar(STRATEGY_CANVAS_NAME, StrategyWorkspaceTabFlags)) {
            for (const auto& [name, portfolio] : _portfolioContainer) {
                ImGui::PushID(name.data());
                portfolio->Paint();
                if (portfolio->Closed()) {
                    _deleteWorkspace = name;
                    _fileName        = FORMAT("Save/{}.json", portfolio->GetName());
                }
                ImGui::PopID();
            }
            ImGui::EndTabBar();
        }

        if (not _deleteWorkspace.empty()) {
            _portfolioContainer.erase(_deleteWorkspace);
            _deleteWorkspace.clear();
            Exports(STRATEGY_CONFIG_FILE_NAME);
            std::remove(_fileName.data());
        }
    }
    ImGui::End();
}
