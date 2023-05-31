#include "../include/StrategyWorkspace.hpp"

#include <nlohmann/json.hpp>

#include "../include/Configuration.hpp"
#include "../include/Logger.hpp"
#include "../include/Portfolio.hpp"
#include "../include/PreCompileHeader.hpp"

extern StrategyNameListT StrategyNameList;

#define STRATEGY_CONFIG_FILE_NAME		 "Config/StrategyWorkspace.json"
#define CREATE_NEW_WORKSPACE_WINDOW_NAME "Add New Workspace"
#define STRATEGY_CANVAS_NAME			 "Workspace Canvas"

StrategyWorkspace::StrategyWorkspace(boost::asio::io_context::strand& strand_) : _strand(strand_) { Imports(STRATEGY_CONFIG_FILE_NAME); }

void StrategyWorkspace::paint(bool* show_) {
	if (*show_) {
		DrawWindow(show_);
	}
}

void StrategyWorkspace::DrawAddNewWorkspace() {
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal(CREATE_NEW_WORKSPACE_WINDOW_NAME, &_newWorkspace, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::InputTextWithHint("Workspace Name", "Set Workspace Name", _strategyWorkspaceName, STRATEGY_NAME_LENGTH);
		if (ImGui::BeginCombo("Strategy List", _strategyListIndex.data())) {
			for (const auto& item : StrategyNameList) {
				if (ImGui::Selectable(item.data())) {
					_strategyListIndex = item;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::BeginDisabled(not strlen(_strategyWorkspaceName));
		if (ImGui::Button(ICON_MD_DONE " Submit")) {
			_portfolioContainer.emplace(_strategyWorkspaceName, std::make_shared<Portfolio>(_strategyWorkspaceName, _strategyListIndex, _strand));
			std::memset(_strategyWorkspaceName, '\0', IM_ARRAYSIZE(_strategyWorkspaceName));
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

void StrategyWorkspace::Exports(std::string_view path_) {
	if (_portfolioContainer.empty()) {
		std::remove(path_.data());
		return;
	}

	nlohmann::ordered_json root;
	std::ranges::for_each(_portfolioContainer, [&](const PortfolioContainerT::value_type& valueType_) { root[valueType_.first.data()] = valueType_.second->getStrategyName(); });

	std::fstream file(path_.data(), std::ios::trunc | std::ios::out);
	if (not file.is_open()) {
		return;
	}
	file << root.dump();
	file.close();
}

void StrategyWorkspace::Imports(std::string_view path_) {
	std::fstream file(path_.data(), std::ios::in);
	if (not file.is_open()) return;

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

		if (_newWorkspace) DrawAddNewWorkspace();
		ImGui::Separator();
		if (ImGui::BeginTabBar(STRATEGY_CANVAS_NAME, StrategyWorkspaceTabFlags)) {
			for (const auto& [name, portfolio] : _portfolioContainer) {
				ImGui::PushID(name.data());
				portfolio->paint();
				if (portfolio->closed()) {
					_deleteWorkspace = name;
					_fileName		 = fmt::format("Save/{}.json", portfolio->getName());
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
