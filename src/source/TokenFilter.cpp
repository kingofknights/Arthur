#include "TokenFilter.hpp"

#include "ContractInfo.hpp"
#include "Enums.hpp"
#include "Structure.hpp"
#include "imgui.h"

TokenFilter::TokenFilter() : _container(Lancelot::ContractInfo::GetCompeleteContract()) {
    std::set<Lancelot::Exchange> exchange;
    for (const auto& pair : _container) {
        exchange.insert(pair.second->_exchange);
    }
    for (const auto& item : exchange) {
        _exchange.push_back(item);
    }
}

void TokenFilter::Paint() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    if (ImGui::Begin("Filter")) {
        if (ImGui::BeginCombo("Exchange", _exchangeData.data())) {
            for (const auto& item : _exchange) {
                if (ImGui::Selectable(Lancelot::ToString(item).data())) {
                    _exchangeData = Lancelot::ToString(item);

                    std::set<Lancelot::Instrument> instrument;
                    for (const auto& result : _container) {
                        if (item == result.second->_exchange) {
                            _localContainer.push_back(result.second);
                            instrument.insert(result.second->_instType);
                        }
                    }
                    for (const auto& inst : instrument) {
                        _instrument.push_back(inst);
                    }

                    break;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::End();
    }
}
