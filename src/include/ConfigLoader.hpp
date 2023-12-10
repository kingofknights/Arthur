#pragma once
#include "Structure.hpp"

class ConfigLoader : public Singleton {
  public:
    static ConfigLoader& Instance();

    std::string getStrategyColumn(const std::string& strategyName_);
    bool        saveStrategyColumn(const std::string& strategyName_, std::string_view params_);
    void        getStrategyList();

    BookOrderListT getOrderHistory(double orderNumber_);

  private:
    ConfigLoader();
};
