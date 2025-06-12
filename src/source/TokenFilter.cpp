#include "TokenFilter.hpp"

#include "Configuration.hpp"
#include "ContractInfo.hpp"
#include "Enums.hpp"
#include "Logger.hpp"
#include "Structure.hpp"
#include "imgui.h"
#include "imgui_internal.h"

#include <fmt/chrono.h>

#include <cfloat>
#include <string>

TokenFilter::TokenFilter() : _container(Lancelot::ContractInfo::GetCompeleteContract()) {
    std::set<Lancelot::Exchange> exchange;
    for (const auto& pair : _container) {
        exchange.insert(pair.second->_exchange);
    }
    for (const auto& item : exchange) {
        _exchange.push_back(item);
    }
}

void TokenFilter::Paint(bool& show_, std::string& contract_) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    if (ImGui::Begin("Token Search and Filter", &show_)) {
        // ImGui::Columns(2);
        DrawExchangeFilter();
        DrawInstrumentFilter();
        DrawSymbolFilter();
        DrawExpiryFilter();
        DrawOptionFilter();
        DrawStikeFilter();
        // ImGui::NextColumn();
        DrawTokenList(show_, contract_);
        // ImGui::EndColumns();
        ImGui::End();
    }
}
void TokenFilter::DrawExchangeFilter() noexcept {
    if (ImGui::BeginCombo("Exchange", _exchangeData.data())) {
        for (const auto& item : _exchange) {
            ImGui::PushID(item);
            if (ImGui::Selectable(Lancelot::ToString(item).data())) {
                _exchangeData = Lancelot::ToString(item);
                _instrumentData.clear();
                _symbolData.clear();
                _expiryData.clear();

                _instrument.clear();
                _symbol.clear();
                _expiry.clear();
                _optionData.clear();
                _strikeData = 0;
                _option.clear();
                _strike.clear();
                _localContainer.clear();

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
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}
void TokenFilter::DrawInstrumentFilter() noexcept {
    if (ImGui::BeginCombo("Instrument", _instrumentData.data())) {
        for (const auto& item : _instrument) {
            ImGui::PushID(item);
            if (ImGui::Selectable(Lancelot::ToString(item).data())) {
                _instrumentData     = Lancelot::ToString(item);
                const auto exchange = Lancelot::ContractInfo::GetExchange(_exchangeData);

                _symbolData.clear();
                _expiryData.clear();
                _symbol.clear();
                _expiry.clear();
                _optionData.clear();
                _strikeData = 0;
                _option.clear();
                _strike.clear();
                _localContainer.clear();
                std::set<std::string> symbol;
                for (const auto& [key, result] : _container) {
                    if (item == result->_instType and result->_exchange == exchange) {
                        _localContainer.push_back(result);
                        symbol.insert(result->_symbol);
                    }
                }
                for (const auto& inst : symbol) {
                    _symbol.push_back(inst);
                }
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}
void TokenFilter::DrawSymbolFilter() noexcept {
    if (ImGui::BeginCombo("Symbol", _symbolData.data())) {
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
            _filter.Clear();
        }
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
        _filter.Draw("##Filter");
        for (const auto& item : _symbol) {
            ImGui::PushID(item.data());
            if (_filter.PassFilter(item.data()) and ImGui::Selectable((item).data())) {
                _symbolData           = item;
                const auto exchange   = Lancelot::ContractInfo::GetExchange(_exchangeData);
                const auto instrument = Lancelot::ContractInfo::GetInstrumentType(_instrumentData);

                _expiryData.clear();
                _expiry.clear();
                _optionData.clear();
                _strikeData = 0;
                _option.clear();
                _strike.clear();
                _localContainer.clear();
                std::set<std::string> expiry;
                for (const auto& [token, result] : _container) {
                    if (exchange == result->_exchange and instrument == result->_instType and item == result->_symbol) {
                        _localContainer.push_back(result);
                        expiry.insert(FORMAT("{:%d %b %Y}", fmt::localtime(result->_expiryDate)));
                    }
                }
                for (const auto& inst : expiry) {
                    _expiry.push_back(inst);
                }
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}
void TokenFilter::DrawExpiryFilter() noexcept {
    if (ImGui::BeginCombo("Expiry", _expiryData.data())) {
        for (const auto& item : _expiry) {
            ImGui::PushID(item.data());
            if (ImGui::Selectable((item).data())) {
                _expiryData           = item;
                const auto exchange   = Lancelot::ContractInfo::GetExchange(_exchangeData);
                const auto instrument = Lancelot::ContractInfo::GetInstrumentType(_instrumentData);

                _optionData.clear();
                _strikeData = 0;
                _option.clear();
                _strike.clear();

                _localContainer.clear();
                std::set<Lancelot::OptionType> expiry;
                for (const auto& [token, result] : _container) {
                    if (exchange == result->_exchange and instrument == result->_instType and _symbolData == result->_symbol and _expiryData == FORMAT("{:%d %b %Y}", fmt::localtime(result->_expiryDate))) {
                        _localContainer.push_back(result);
                        expiry.insert(result->_option);
                    }
                }
                for (const auto& inst : expiry) {
                    _option.push_back(inst);
                }
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}
void TokenFilter::DrawOptionFilter() noexcept {
    if (ImGui::BeginCombo("Option", _optionData.data())) {
        for (const auto& item : _option) {
            ImGui::PushID(item);
            if (ImGui::Selectable(Lancelot::ToString(item).data())) {
                _optionData           = Lancelot::ToString(item);
                const auto exchange   = Lancelot::ContractInfo::GetExchange(_exchangeData);
                const auto instrument = Lancelot::ContractInfo::GetInstrumentType(_instrumentData);

                _strikeData = 0;
                _strike.clear();

                _localContainer.clear();
                std::set<float> expiry;
                for (const auto& [token, result] : _container) {
                    if (exchange == result->_exchange and instrument == result->_instType and _symbolData == result->_symbol and _expiryData == FORMAT("{:%d %b %Y}", fmt::localtime(result->_expiryDate)) and _optionData == Lancelot::ToString(result->_option)) {
                        _localContainer.push_back(result);
                        expiry.insert(result->_strikePrice);
                    }
                }
                for (const auto& inst : expiry) {
                    _strike.push_back(inst);
                }
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}

void TokenFilter::DrawStikeFilter() noexcept {
    if (ImGui::BeginCombo("Strike", FORMAT("{}", _strikeData).data())) {
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
            _filterStrike.Clear();
        }
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
        _filterStrike.Draw("##Filter");
        for (const auto& item : _strike) {
            ImGui::PushID(static_cast<int>(item));
            const std::string data = FORMAT("{:.2f}", item);
            if (_filterStrike.PassFilter(data.data()) and ImGui::Selectable(data.data())) {
                _strikeData           = item;
                const auto exchange   = Lancelot::ContractInfo::GetExchange(_exchangeData);
                const auto instrument = Lancelot::ContractInfo::GetInstrumentType(_instrumentData);

                _localContainer.clear();
                for (const auto& [token, result] : _container) {
                    if (exchange == result->_exchange and instrument == result->_instType and _symbolData == result->_symbol and _expiryData == FORMAT("{:%d %b %Y}", fmt::localtime(result->_expiryDate)) and _optionData == Lancelot::ToString(result->_option) and _strikeData == result->_strikePrice) {
                        _localContainer.push_back(result);
                    }
                }
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}
void TokenFilter::DrawTokenList(bool& show_, std::string& contract_) noexcept {
    if (ImGui::BeginListBox("##FilterTokenList", ImVec2(-FLT_MIN, -FLT_MIN))) {
        for (const auto* const item : _localContainer) {
            ImGui::PushID(item->_token);
            if (ImGui::Selectable(item->_description.data())) {
                show_     = false;
                contract_ = item->_description;
            }
            ImGui::PopID();
        }
        ImGui::EndListBox();
    }
}
