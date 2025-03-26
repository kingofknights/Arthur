#pragma once

#include "PortfolioScanner.hpp"
#include "Structure.hpp"

#include <boost/asio.hpp>

#include <cstdint>
#include <functional>

using StrategyActionT = std::function<void(StrategyRowPtrT, std::string, Lancelot::RequestType)>;

class PortfolioInterface : public PortfolioScanner {
  public:
    explicit PortfolioInterface(const std::string& name_, std::string_view strategyName_, boost::asio::io_context::strand& strand_);

    ~PortfolioInterface();

    static void setStrategyActionCallback(StrategyActionT StrategyAction_);

  protected:
    static StrategyActionT StrategyAction;

    static auto GetStatusColor(StrategyStatus status_, bool changed_) -> ImVec4;

    void subscribeAll();
    void subscribeSelected();
    void applyAll();
    void applySelected();
    void unsubscribeAll();
    void unsubscribeSelected();
    void updateAll(GlobalParameterInfoT& info_);

    void Exports(const std::string& path_);
    void Imports(const std::string& path_);

    void ParseConfig(std::string_view config_);
    void doStrategyAction(const StrategyRowPtrT& strategy_, const std::string& name_, Lancelot::RequestType type_);

    PortfolioStatusT checkAnyActive();

  protected:
    enum {
        MAX_PORTFOLIO_ALLOWED = 9999
    };
    static uint32_t PortFolioNumber;

    bool _open            = true;
    bool _exportActivated = false;

    const std::string _name;
    const std::string _strategyName;
    GlobalParamListT  _globalParamList;

    boost::asio::io_context::strand& _strand;
};
