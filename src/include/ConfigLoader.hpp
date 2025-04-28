#pragma once
#include "Structure.hpp"

class ConfigLoader : public Singleton {
  public:
    static auto Instance() -> ConfigLoader&;

    static auto GetStrategyColumn(const std::string& strategyName_) -> std::string;

    static auto SaveStrategyColumn(const std::string& strategyName_, std::string_view params_) -> bool;

    void GetStrategyList();

    static auto GetOrderHistory(uint64_t orderNumber_) -> BookOrderListT;

  private:
    ConfigLoader();
};
