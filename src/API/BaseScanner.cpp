#include "BaseScanner.hpp"

#include <Lancelot/ContractInfo/ContractInfo.hpp>
#include <Lancelot/Logger/Logger.hpp>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>

#include "../API/ContractInfo.hpp"
#include "../include/Enums.hpp"
#include "../include/Portfolio.hpp"
#include "../include/Structure.hpp"

typedef std::vector<std::string> SplitT;

extern ScannerContainerT                ScannerContainer;
extern GlobalPortfolioScannerContainerT GlobalPortfolioScannerContainer;

void BaseScanner::RegisterScanner(double PF_) {
    LOG(INFO, "{} {}", __FUNCTION__, PF)
    auto iterator = ScannerContainer.find(PF_);
    if (iterator != ScannerContainer.end()) {
        iterator->second.BaseScannerPtr = shared_from_this();
    } else {
        ScannerPointerT scannerPointer;
        scannerPointer.BaseScannerPtr = shared_from_this();
        auto pair                     = ScannerContainer.emplace(PF_, scannerPointer);

        LOG(INFO, "{} {} {}", __FUNCTION__, PF_, pair.second)
    }
}

void BaseScanner::UpdateUser(double PF_, const std::string& update) {
    auto iterator = GlobalPortfolioScannerContainer.find(PF_);
    if (iterator != GlobalPortfolioScannerContainer.end()) {
        auto&  ScannerResult = iterator->second;
        SplitT split;
        boost::split(split, update, boost::is_any_of("#"));
        for (const auto& keyValue : split) {
            SplitT split_;
            boost::split(split_, keyValue, boost::is_any_of("="));
            if (split_.size() == 2) {
                const std::string& key   = split_[0];
                const std::string& value = split_[1];
                auto               item  = ScannerResult.ParameterInfoList.find(key);
                if (item != ScannerResult.ParameterInfoList.end()) {
                    auto& parameter = item->second.Parameter;
                    switch (item->second.Type) {
                        case DataType_CONTRACT: {
                            int token         = stoi(value);
                            parameter.Text    = Lancelot::ContractInfo::GetDescription(token);
                            item->second.Self = ContractInfo::GetLiveDataRef(token);
                            break;
                        }
                        case DataType_INT: {
                            parameter.Integer = stoi(value);
                            break;
                        }
                        case DataType_FLOAT: {
                            parameter.Floating = stod(value);
                            break;
                        }
                        case DataType_CLIENT:
                        case DataType_TEXT: {
                            parameter.Text = value;
                            break;
                        }
                        case DataType_RADIO: {
                            parameter.Check = value == "true";
                            break;
                        }
                        case DataType_COMBO:
                        case DataType_UPDATES:
                        case DataType_END:
                            break;
                    }
                }
            }
        }

        ScannerResult.PortfolioPtr->AddScannerPortfolio(ScannerResult.ParameterInfoList);
    }
}

BaseScanner::BaseScanner(double PF_, double UID_) : PF(PF_), UID(UID_) {
    RegisterScanner(PF);
}
