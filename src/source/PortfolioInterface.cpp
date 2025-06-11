#include "PortfolioInterface.hpp"

#include "API/Common.hpp"
#include "API/ContractInfo.hpp"
#include "Colors.hpp"
#include "ConfigLoader.hpp"
#include "Enums.hpp"
#include "Structure.hpp"
#include "Utils.hpp"
#include "include/Arthur_Fwd.hpp"

#include <nlohmann/json.hpp>

#include <boost/algorithm/string.hpp>

#include <cstdint>
#include <fstream>
#include <future>
#include <utility>

extern std::string StatusDisplay;

uint32_t                            PortfolioInterface::PortFolioNumber = 0;
PortfolioInterface::StrategyActionT PortfolioInterface::StrategyAction;
AddContractFunctionT                PortfolioInterface::AddContractFunction;

PortfolioInterface::PortfolioInterface(std::string name_, const std::string& strategyName_, ExecutorStrandT& strand_)
    : PortfolioScanner(strategyName_),
      _name(std::move(name_)),
      _strategyName(strategyName_),
      _strand(strand_) {
    const std::string jsonData = ConfigLoader::Instance().GetStrategyColumn(_strategyName);
    ParseConfig(jsonData);
    PortfolioInterface::Imports(FORMAT("Save/{}.json", _name));
    PortfolioScanner::Import(FORMAT("Save/{}_Scanner.json", _name));
}

PortfolioInterface::~PortfolioInterface() {
    if (_open) {
        PortfolioInterface::Exports(FORMAT("Save/{}.json", _name));
        PortfolioScanner::Export(FORMAT("Save/{}_Scanner.json", _name));
    }
}

auto PortfolioInterface::GetStatusColor(StrategyStatus status_, bool changed_) noexcept -> ImVec4 {
    switch (status_) {
        case StrategyStatus_PENDING:
        case StrategyStatus_WAITING:
        case StrategyStatus_INACTIVE: {
            return COLOR_GRAY;
        }
        case StrategyStatus_APPLIED:
        case StrategyStatus_ACTIVE: {
            return changed_ ? COLOR_YELLOW : COLOR_GREEN;
        }
        case StrategyStatus_DISCONNECTED:
        case StrategyStatus_TERMINATED: {
            return COLOR_RED;
        }
    }
    return COLOR_BLUE;
}

void PortfolioInterface::SubscribeAll() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if (value_type->_changed or value_type->_status == StrategyStatus_TERMINATED or value_type->_status == StrategyStatus_INACTIVE) {
                value_type->_status = StrategyStatus_WAITING;
                DoStrategyAction(value_type, _strategyName, Lancelot::RequestType_SUBSCRIBE);
            }
        }
    });
}

void PortfolioInterface::SubscribeSelected() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if ((value_type->_changed or value_type->_status == StrategyStatus_TERMINATED or value_type->_status == StrategyStatus_INACTIVE) and value_type->_selected) {
                value_type->_status = StrategyStatus_WAITING;
                DoStrategyAction(value_type, _strategyName, Lancelot::RequestType_SUBSCRIBE);
            }
        }
    });
}

void PortfolioInterface::ApplySelected() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if ((value_type->_changed or value_type->_status == StrategyStatus_ACTIVE) and value_type->_subscribed and value_type->_selected) {
                DoStrategyAction(value_type, _strategyName, Lancelot::RequestType_APPLY);
            }
        }
    });
}

void PortfolioInterface::ApplyAll() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if ((value_type->_changed or value_type->_status == StrategyStatus_ACTIVE) and value_type->_subscribed) {
                DoStrategyAction(value_type, _strategyName, Lancelot::RequestType_APPLY);
            }
        }
    });
}

void PortfolioInterface::UnsubscribeAll() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if (value_type->_changed or value_type->_status == StrategyStatus_ACTIVE or value_type->_status == StrategyStatus_APPLIED) {
                DoStrategyAction(value_type, _strategyName, Lancelot::RequestType_UNSUBSCRIBE);
            }
        }
    });
}

void PortfolioInterface::UnsubscribeSelected() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if ((value_type->_changed or value_type->_status == StrategyStatus_ACTIVE or value_type->_status == StrategyStatus_APPLIED) and value_type->_selected) {
                DoStrategyAction(value_type, _strategyName, Lancelot::RequestType_UNSUBSCRIBE);
            }
        }
    });
}
void PortfolioInterface::Exports(const std::string& path_) {
    if (_strategyList.empty()) {
        std::remove(path_.data());
        return;
    }
    _exportActivated = true;
    nlohmann::ordered_json root;
    for (const StrategyRowPtrT& strategyRow : _strategyList) {
        nlohmann::ordered_json list;
        for (const ParameterInfoListT::value_type& value_type : strategyRow->_parameterInfoList) {
            const std::string_view parameterName  = value_type.first;
            const ParameterInfoT&  parameterValue = value_type.second;
            nlohmann::ordered_json config;
            switch (parameterValue._type) {
                case DataType_INT: {
                    config["Value"] = FORMAT("{}", parameterValue._parameter._integer);
                    break;
                }
                case DataType_FLOAT: {
                    config["Value"] = FORMAT("{}", parameterValue._parameter._floating);
                    break;
                }
                case DataType_RADIO: {
                    config["Value"] = FORMAT("{:d}", parameterValue._parameter._check);
                    break;
                }
                case DataType_COMBO:
                case DataType_CLIENT:
                case DataType_UPDATES:
                case DataType_TEXT:
                case DataType_CONTRACT: {
                    config["Value"] = parameterValue._parameter._text;
                    break;
                }
                case DataType_END: {
                    continue;
                }
            }

            config["Type"]      = parameterValue._type;
            list[parameterName] = config;
        }
        root.push_back(list);
    }
    _exportActivated = false;
    std::fstream file(path_.data(), std::ios::trunc | std::ios::out);
    file << root.dump();
    file.close();
    StatusDisplay = FORMAT("Exporting done : {} {}", path_, _strategyList.size());
}

void PortfolioInterface::Imports(const std::string& path_) {
    std::fstream file(path_, std::ios::in);
    if (not file.is_open()) {
        return;
    }
    nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);

    for (const auto& item : root.items()) {
        ParameterInfoListT parameterInfoList;
        bool               add = true;
        for (const auto& config : item.value().items()) {
            const auto& value = config.value();

            ParameterInfoT parameterInfo;
            parameterInfo._type        = value.at("Type").get<DataType>();
            std::string parameterValue = value.at("Value").get<std::string>();
            switch (parameterInfo._type) {
                case DataType_INT: {
                    parameterInfo._parameter._integer = std::stoi(parameterValue);
                    break;
                }
                case DataType_FLOAT: {
                    parameterInfo._parameter._floating = std::stof(parameterValue);
                    break;
                }
                case DataType_COMBO:
                case DataType_CLIENT:
                case DataType_UPDATES:
                case DataType_TEXT: {
                    parameterInfo._parameter._text = parameterValue;
                    break;
                }
                case DataType_RADIO: {
                    parameterInfo._parameter._check = parameterValue == "1";
                    break;
                }
                case DataType_CONTRACT: {
                    parameterInfo._parameter._text = parameterValue;

                    auto token                 = Lancelot::ContractInfo::GetToken(parameterValue);
                    parameterInfo._marketWatch = ContractInfo::GetLiveDataRef(token);
#if REMOVE_EXPIRED_TOKEN
                    if (token == -1) {
                        add = false;
                    } else {
                        add = ContractInfo::GetExpiry(token) < time(nullptr);
                    }
#endif
                    break;
                }
                case DataType_END: {
                    break;
                }
            }
#if REMOVE_EXPIRED_TOKEN
            if (not add) {
                break;
            }
#endif

            parameterInfoList.emplace(config.key(), parameterInfo);
        }
        if (add) {
            StrategyRowPtrT strategyRow     = std::make_shared<StrategyRowT>();
            strategyRow->_changed           = false;
            strategyRow->_subscribed        = false;
            strategyRow->_portfolio         = ++PortFolioNumber;
            strategyRow->_status            = StrategyStatus_INACTIVE;
            strategyRow->_parameterInfoList = parameterInfoList;
            _strategyList.emplace_back(strategyRow);
            Utils::AppendPortfolio(strategyRow->_portfolio, strategyRow);
        }
    }
    StatusDisplay = FORMAT("Importing done : {} {}", path_, _strategyList.size());
    file.close();
}

void PortfolioInterface::UpdateAll(GlobalParameterInfoT& info_) {
    std::ranges::for_each(_strategyList, [&info_](const StrategyListT::value_type& value) {
        auto iterator = value->_parameterInfoList.find(info_._name);
        if (iterator != value->_parameterInfoList.end()) {
            bool status = (value->_status == StrategyStatus_ACTIVE || value->_status == StrategyStatus_APPLIED);

            if (status) {
                value->_changed = true;
            }

            ParameterValueT& parameterValue = iterator->second._parameter;
            switch (info_._parameterInfo._type) {
                case DataType_INT: {
                    parameterValue._integer = info_._parameterInfo._parameter._integer;
                    break;
                }
                case DataType_FLOAT: {
                    parameterValue._floating = info_._parameterInfo._parameter._floating;
                    break;
                }
                case DataType_TEXT: {
                    parameterValue._text = info_._parameterInfo._parameter._text;

                    break;
                }
                case DataType_RADIO: {
                    parameterValue._check = info_._parameterInfo._parameter._check;
                    break;
                }
                case DataType_COMBO: {
                    std::vector<std::string> result;
                    boost::split(result, info_._parameterInfo._parameter._text, boost::is_any_of(";"));
                    parameterValue._text = result.at(info_._parameterInfo._parameter._integer);

                    break;
                }
                case DataType_CLIENT: {
                    parameterValue._text = info_._parameterInfo._parameter._text;
                    break;
                }
                case DataType_UPDATES:
                case DataType_CONTRACT:
                case DataType_END: {
                    break;
                }
            }
        }
    });
}

PortfolioStatusT PortfolioInterface::CheckAnyActive() {
    PortfolioStatusT status{false, 0, 0, 0, 0, 0};

    std::ranges::for_each(_strategyList, [&](const StrategyListT::value_type& valueType_) {
        switch (valueType_->_status) {
            case StrategyStatus_INACTIVE: {
                status._inactive += 1;
                break;
            }
            case StrategyStatus_ACTIVE: {
                status._active += 1;
                break;
            }
            case StrategyStatus_APPLIED: {
                status._apply += 1;
                break;
            }
            case StrategyStatus_TERMINATED: {
                status._terminate += 1;
                break;
            }
            case StrategyStatus_WAITING: {
                status._waiting += 1;
                break;
            }
            case StrategyStatus_DISCONNECTED: {
                break;
            }
        }
    });

    status._close = status._active || status._apply || status._waiting;
    return status;
}

void PortfolioInterface::ParseConfig(std::string_view config_) {
    nlohmann::ordered_json json        = nlohmann::ordered_json::parse(config_);
    auto                   paramConfig = json["Params"];
    _paramList.clear();
    for (const auto& item : paramConfig.items()) {
        const auto&    value = item.value();
        ParameterInfoT param;
        param._type      = static_cast<DataType>(value["DataType"].get<int>());
        std::string data = value["Value"].get<std::string>();

        switch (param._type) {
            case DataType_CLIENT:
            case DataType_UPDATES:
            case DataType_COMBO:
            case DataType_TEXT:
            case DataType_CONTRACT: {
                param._parameter._text = data;
                break;
            }
            case DataType_INT: {
                param._parameter._integer = std::stoi(data);
                break;
            }
            case DataType_FLOAT: {
                param._parameter._floating = std::stof(data);
                break;
            }
            case DataType_RADIO: {
                param._parameter._check = std::stoi(data);
                break;
            }
            case DataType_END: {
                continue;
            }
        }
        _paramList.insert(ParameterInfoListT ::value_type(item.key(), param));
        GlobalParameterInfoT global{._update = false, ._name = item.key(), ._parameterInfo = param};
        _globalParamList.push_back(global);
    }
}

void PortfolioInterface::DoStrategyAction(const StrategyRowPtrT& strategy_, const std::string& name_, Lancelot::RequestType type_) {
    strategy_->_status = StrategyStatus_PENDING;
    _strand.post([strategy_, type_, name_]() { StrategyAction(strategy_, name_, type_); });
}

auto PortfolioInterface::GetStrategyName() const noexcept -> std::string {
    return _strategyName;
}

auto PortfolioInterface::GetName() const noexcept -> std::string {
    return _name;
}

auto PortfolioInterface::GetMaxPortfolio() noexcept -> uint32_t {
    return MaxPortfolioAllowed;
}
auto PortfolioInterface::GetPortfolio() noexcept -> uint32_t {
    return PortFolioNumber;
}
auto PortfolioInterface::IsOpen() const noexcept -> bool {
    return _open;
}
