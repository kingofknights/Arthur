#include "../include/ConfigLoader.hpp"

#include <filesystem>
#include <fstream>

#include "../include/Structure.hpp"

extern StrategyNameListT StrategyNameList;

ConfigLoader::ConfigLoader() {
    LOG(INFO, "{}", __FUNCTION__)
    const std::string path = "Save/";
    if (not std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
    getStrategyList();
}

ConfigLoader& ConfigLoader::Instance() {
    static ConfigLoader config;
    return config;
}

std::string ConfigLoader::getStrategyColumn(const std::string& strategyName_) {
#if LOAD_FROM_FILE
    std::ifstream file("Save/" + strategyName_ + ".json");
    if (file.is_open()) {
        std::stringstream strStream;
        strStream << file.rdbuf();
        return strStream.str();
    }
#else
    auto table = Lancelot::ContractInfo::GetResultWithIndex(FORMAT(GetStrategyColumns_, strategyName_));
    for (const auto& row : table) {
        for (const auto& item : row) return item;
    }

#endif
    return {};
}

bool ConfigLoader::saveStrategyColumn(const std::string& strategyName_, std::string_view params_) {
#if LOAD_FROM_FILE
    std::ofstream file("Save/" + strategyName_ + ".json", std::ios::trunc);
    if (file.is_open()) {
        file << params_;
        file.close();
        return true;
    }
    return false;
#else
    Lancelot::ContractInfo::ExecuteQuery(FORMAT(RemoveStrategy_, strategyName_));
    Lancelot::ContractInfo::ExecuteQuery(FORMAT(InsertStrategy_, strategyName_, params_));
    StrategyNameList.push_back(strategyName_);
#endif
    return true;
}

void ConfigLoader::getStrategyList() {
#if LOAD_FROM_FILE
    std::string path = "Save/";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::string filename = entry.path().filename().string();
        filename             = filename.substr(0, filename.length() - entry.path().extension().string().length());
        char* file           = new char[filename.length() + 1];

        std::memset(file, '\0', filename.length() + 1);
        std::memcpy(file, filename.c_str(), filename.length());
        StrategyNameList.push_back(file);
    }
#else
    auto table = Lancelot::ContractInfo::GetResultWithIndex(GetStrategyList_);
    for (const auto& row : table) {
        StrategyNameList.push_back(row[0]);
    }
#endif
}

BookOrderListT ConfigLoader::getOrderHistory(double orderNumber_) {
    return {};
}
