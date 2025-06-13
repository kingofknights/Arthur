#pragma once

#include "Arthur_Fwd.hpp"
#include "PortfolioScanner.hpp"
#include "Structure.hpp"

#include <cstdint>

class PortfolioInterface
    : public PortfolioScanner {
    using StrategyActionT = std::function<void(StrategyRowPtrT, std::string, Lancelot::RequestType)>;

    friend class Arthur;
    friend class Portfolio;

  public:
    explicit PortfolioInterface(std::string name_, const std::string& strategyName_, ExecutorStrandT& strand_);

    ~PortfolioInterface();

    [[nodiscard]] auto GetStrategyName() const noexcept -> std::string;

    [[nodiscard]] auto GetName() const noexcept -> std::string;

    [[nodiscard]] static auto GetMaxPortfolio() noexcept -> uint32_t;

    [[nodiscard]] static auto GetPortfolio() noexcept -> uint32_t;

    [[nodiscard]] auto IsOpen() const noexcept -> bool;

  protected:
    static auto GetStatusColor(StrategyStatus status_, bool changed_) noexcept -> ImVec4;

    void SubscribeAll();

    void SubscribeSelected();

    void ApplyAll();

    void ApplySelected();

    void UnsubscribeAll();

    void UnsubscribeSelected();

    void UpdateAll(GlobalParameterInfoT& info_);

    void Exports(const std::string& path_);

    void Imports(const std::string& path_);

    void ExportsCsv(const std::string& path_);

    void ImportsCsv(const std::string& path_);

    void ParseConfig(std::string_view config_);

    void DoStrategyAction(const StrategyRowPtrT& strategy_, const std::string& name_, Lancelot::RequestType type_);

    auto CheckAnyActive() -> PortfolioStatusT;

  private:
    const std::string _name;
    const std::string _strategyName;
    ExecutorStrandT&  _strand;

    static constexpr uint32_t MaxPortfolioAllowed = 9999;
    static uint32_t           PortFolioNumber;

    static StrategyActionT      StrategyAction;
    static AddContractFunctionT AddContractFunction;

    bool _open            = true;
    bool _exportActivated = false;

    GlobalParamListT _globalParamList;
};
