#pragma once

#include <boost/asio.hpp>
#include <functional>

#include "PortfolioScanner.hpp"
#include "Structure.hpp"

using StrategyActionT = std::function<void(StrategyRowPtrT, std::string, Lancelot::RequestType)>;

class PortfolioInterface : public PortfolioScanner {
  public:
    explicit PortfolioInterface(std::string_view name_, std::string_view strategyName_, boost::asio::io_context::strand& strand_);

    ~PortfolioInterface();

    static void setStrategyActionCallback(StrategyActionT StrategyAction_);

  protected:
    static StrategyActionT StrategyAction;
    static ImVec4          GetStatusColor(StrategyStatus status_, bool changed_);

    void subscribeAll();
    void subscribeSelected();
    void applyAll();
    void applySelected();
    void unsubscribeAll();
    void unsubscribeSelected();
    void updateAll(GlobalParameterInfoT& info_);
    void Exports(std::string_view path_);
    void Imports(std::string_view path_);
    void ParseConfig(std::string_view config_);
    void doStrategyAction(const StrategyRowPtrT& strategy_, const std::string& name_, Lancelot::RequestType type_);

    PortfolioStatusT checkAnyActive();

  protected:
    enum {
        MAX_PORTFOLIO_ALLOWED = 9999
    };
    static int _portFolioNumber;
    bool       _open            = true;
    bool       _exportActivated = false;

    const std::string _name;
    const std::string _strategyName;
    GlobalParamListT  _globalParamList;

    boost::asio::io_context::strand& _strand;
};
