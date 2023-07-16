#include "../include/PortfolioInterface.hpp"

#include <boost/algorithm/string.hpp>
#include <future>
#include <nlohmann/json.hpp>

#include "../API/Common.hpp"
#include "../API/ContractInfo.hpp"
#include "../include/Colors.hpp"
#include "../include/ConfigLoader.hpp"
#include "../include/Enums.hpp"
#include "../include/Structure.hpp"
#include "../include/Utils.hpp"

extern std::string StatusDisplay;

int				PortfolioInterface::_portFolioNumber = 0;
StrategyActionT PortfolioInterface::StrategyAction;

PortfolioInterface::PortfolioInterface(std::string_view name_, std::string_view strategyName_, boost::asio::io_context::strand& strand_)
	: PortfolioScanner(strategyName_.data()), _name(name_), _strategyName(strategyName_), _strand(strand_) {
	const std::string jsonData = ConfigLoader::Instance().getStrategyColumn(_strategyName);
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

ImVec4 PortfolioInterface::GetStatusColor(StrategyStatus status_, bool changed_) {
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

void PortfolioInterface::subscribeAll() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if (value_type->Changed or value_type->Status == StrategyStatus_TERMINATED or value_type->Status == StrategyStatus_INACTIVE) {
                value_type->Status = StrategyStatus_WAITING;
                doStrategyAction(value_type, _strategyName, Lancelot::RequestType_SUBSCRIBE);
            }
        }
    });
}

void PortfolioInterface::subscribeSelected() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if ((value_type->Changed or value_type->Status == StrategyStatus_TERMINATED or value_type->Status == StrategyStatus_INACTIVE) and value_type->Selected) {
                value_type->Status = StrategyStatus_WAITING;
                doStrategyAction(value_type, _strategyName, Lancelot::RequestType_SUBSCRIBE);
            }
        }
    });
}

void PortfolioInterface::applySelected() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if ((value_type->Changed or value_type->Status == StrategyStatus_ACTIVE) and value_type->Subscribed and value_type->Selected) {
                doStrategyAction(value_type, _strategyName, Lancelot::RequestType_APPLY);
            }
        }
    });
}

void PortfolioInterface::applyAll() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if ((value_type->Changed or value_type->Status == StrategyStatus_ACTIVE) and value_type->Subscribed) {
                doStrategyAction(value_type, _strategyName, Lancelot::RequestType_APPLY);
            }
        }
    });
}

void PortfolioInterface::unsubscribeAll() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if (value_type->Changed or value_type->Status == StrategyStatus_ACTIVE or value_type->Status == StrategyStatus_APPLIED) {
                doStrategyAction(value_type, _strategyName, Lancelot::RequestType_UNSUBSCRIBE);
            }
        }
    });
}

void PortfolioInterface::unsubscribeSelected() {
    auto _ = std::async(std::launch::async, [&]() {
        for (const StrategyListT::value_type& value_type : _strategyList) {
            if ((value_type->Changed or value_type->Status == StrategyStatus_ACTIVE or value_type->Status == StrategyStatus_APPLIED) and value_type->Selected) {
                doStrategyAction(value_type, _strategyName, Lancelot::RequestType_UNSUBSCRIBE);
            }
        }
    });
}
void PortfolioInterface::Exports(std::string_view path_) {
    if (_strategyList.empty()) {
        std::remove(path_.data());
        return;
    }
    _exportActivated = true;
    nlohmann::ordered_json root;
    for (const StrategyRowPtrT& strategyRow : _strategyList) {
        nlohmann::ordered_json list;
        for (const ParameterInfoListT::value_type& value_type : strategyRow->ParameterInfoList) {
            const std::string_view parameterName  = value_type.first;
            const ParameterInfoT&  parameterValue = value_type.second;
            nlohmann::ordered_json config;
            switch (parameterValue.Type) {
                case DataType_INT: {
                    config["Value"] = FORMAT("{}", parameterValue.Parameter.Integer);
                    break;
                }
                case DataType_FLOAT: {
                    config["Value"] = FORMAT("{}", parameterValue.Parameter.Floating);
                    break;
                }
                case DataType_RADIO: {
                    config["Value"] = FORMAT("{:d}", parameterValue.Parameter.Check);
                    break;
                }
                case DataType_COMBO:
                case DataType_CLIENT:
                case DataType_UPDATES:
                case DataType_TEXT:
                case DataType_CONTRACT: {
                    config["Value"] = parameterValue.Parameter.Text;
                    break;
                }
                case DataType_END: {
                    continue;
                }
            }

			config["Type"]		= parameterValue.Type;
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

void PortfolioInterface::Imports(std::string_view path_) {
    std::fstream file(path_.data(), std::ios::in);
    if (not file.is_open()) {
        return;
    }
    nlohmann::ordered_json root = nlohmann::ordered_json::parse(file);

    ParameterInfoListT parameterInfoList;
    for (const auto& item : root.items()) {
        ParameterInfoT parameterInfo;
        bool		   add = true;
        for (const auto& config : item.value().items()) {
            const auto& value = config.value();

            parameterInfo.Type		   = value.at("Type").get<DataType>();
            std::string parameterValue = value.at("Value").get<std::string>();
            switch (parameterInfo.Type) {
                case DataType_INT: {
                    parameterInfo.Parameter.Integer = std::stoi(parameterValue);
                    break;
                }
                case DataType_FLOAT: {
                    parameterInfo.Parameter.Floating = std::stof(parameterValue);
                    break;
                }
                case DataType_COMBO:
                case DataType_CLIENT:
                case DataType_UPDATES:
                case DataType_TEXT: {
                    parameterInfo.Parameter.Text = parameterValue;
                    break;
                }
                case DataType_RADIO: {
                    parameterInfo.Parameter.Check = parameterValue == "1";
                    break;
                }
                case DataType_CONTRACT: {
                    parameterInfo.Parameter.Text = parameterValue;

                    auto token		   = Lancelot::ContractInfo::GetToken(parameterValue);
                    parameterInfo.Self = ContractInfo::GetLiveDataRef(token);
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
			StrategyRowPtrT strategyRow	   = std::make_shared<StrategyRowT>();
			strategyRow->Changed		   = false;
			strategyRow->Subscribed		   = false;
			strategyRow->PF				   = ++_portFolioNumber;
			strategyRow->Status			   = StrategyStatus_INACTIVE;
			strategyRow->ParameterInfoList = parameterInfoList;
			_strategyList.emplace_back(strategyRow);
			Utils::AppendPortfolio(strategyRow->PF, strategyRow);
		}
	}
	StatusDisplay = FORMAT("Importing done : {} {}", path_, _strategyList.size());
	file.close();

}

void PortfolioInterface::updateAll(GlobalParameterInfoT& info_) {
    std::ranges::for_each(_strategyList, [&info_](const StrategyListT::value_type& value) {
        auto iterator = value->ParameterInfoList.find(info_.Name);
        if (iterator != value->ParameterInfoList.end()) {
            bool status = (value->Status == StrategyStatus_ACTIVE || value->Status == StrategyStatus_APPLIED);

            if (status) {
                value->Changed = true;
            }

            ParameterValueT& parameterValue = iterator->second.Parameter;
            switch (info_.Info.Type) {
                case DataType_INT: {
                    parameterValue.Integer = info_.Info.Parameter.Integer;
                    break;
                }
                case DataType_FLOAT: {
                    parameterValue.Floating = info_.Info.Parameter.Floating;
                    break;
                }
                case DataType_TEXT: {
                    parameterValue.Text = info_.Info.Parameter.Text;

                    break;
                }
                case DataType_RADIO: {
                    parameterValue.Check = info_.Info.Parameter.Check;
                    break;
                }
                case DataType_COMBO: {
                    std::vector<std::string> result;
                    boost::split(result, info_.Info.Parameter.Text, boost::is_any_of(";"));
                    parameterValue.Text = result.at(info_.Info.Parameter.Integer);

                    break;
                }
                case DataType_CLIENT: {
                    parameterValue.Text = info_.Info.Parameter.Text;
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

PortfolioStatusT PortfolioInterface::checkAnyActive() {
    PortfolioStatusT status{false, 0, 0, 0, 0, 0};

    std::ranges::for_each(_strategyList, [&](const StrategyListT::value_type& valueType_) {
        switch (valueType_->Status) {
            case StrategyStatus_INACTIVE: {
                status.Inactive += 1;
                break;
            }
            case StrategyStatus_ACTIVE: {
                status.Active += 1;
                break;
            }
            case StrategyStatus_APPLIED: {
                status.Apply += 1;
                break;
            }
            case StrategyStatus_TERMINATED: {
                status.Terminate += 1;
                break;
            }
            case StrategyStatus_WAITING: {
                status.Waiting += 1;
                break;
            }
            case StrategyStatus_DISCONNECTED: {
                break;
            }
        }
    });

    status.Close = status.Active || status.Apply || status.Waiting;
    return status;
}

void PortfolioInterface::ParseConfig(std::string_view config_) {
    nlohmann::ordered_json json		   = nlohmann::ordered_json::parse(config_);
    auto				   paramConfig = json["Params"];
    _paramList.clear();
    for (const auto& item : paramConfig.items()) {
        const auto&	   value = item.value();
        ParameterInfoT param;
        param.Type		 = static_cast<DataType>(value["DataType"].get<int>());
        std::string data = value["Value"].get<std::string>();

        switch (param.Type) {
            case DataType_CLIENT:
            case DataType_UPDATES:
            case DataType_COMBO:
            case DataType_TEXT:
            case DataType_CONTRACT: {
                param.Parameter.Text = data;
                break;
            }
            case DataType_INT: {
                param.Parameter.Integer = std::stoi(data);
                break;
            }
            case DataType_FLOAT: {
                param.Parameter.Floating = std::stof(data);
                break;
            }
            case DataType_RADIO: {
                param.Parameter.Check = std::stoi(data);
                break;
            }
            case DataType_END: {
                continue;
            }
        }
        _paramList.insert(ParameterInfoListT ::value_type(item.key(), param));
        GlobalParameterInfoT global{.Update = false, .Name = item.key(), .Info = param};
        _globalParamList.push_back(global);
    }
}

void PortfolioInterface::doStrategyAction(const StrategyRowPtrT& strategy_, const std::string& name_, Lancelot::RequestType type_) {
    strategy_->Status = StrategyStatus_PENDING;
    _strand.post([strategy_, type_, name_]() { StrategyAction(strategy_, name_, type_); });
}

void PortfolioInterface::setStrategyActionCallback(StrategyActionT StrategyAction_) {
    StrategyAction = std::move(StrategyAction_);
}
