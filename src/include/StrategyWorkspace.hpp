#pragma once

#include "Arthur_Fwd.hpp"
#include "Structure.hpp"

#include <boost/asio.hpp>

class Portfolio;

using PortfolioPtrT       = std::shared_ptr<Portfolio>;
using PortfolioContainerT = std::unordered_map<std::string, PortfolioPtrT>;

class StrategyWorkspace final {
  public:
    explicit StrategyWorkspace(ExecutorT& strand_);

    void Paint(bool* show_);

  protected:
    void DrawWindow(bool* show_);

    void DrawAddNewWorkspace();

    void Exports(const std::string& path_);

    void Imports(const std::string& path_);

  private:
    ExecutorT& _strand;

    PortfolioContainerT _portfolioContainer;

    bool _newWorkspace = false;

    std::string _strategyWorkspaceName;
    std::string _deleteWorkspace;
    std::string _fileName;
    std::string _strategyListIndex;
};
