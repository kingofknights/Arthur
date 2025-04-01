#pragma once

#include "Structure.hpp"

#include <boost/asio.hpp>

class Portfolio;

using PortfolioPtrT       = std::shared_ptr<Portfolio>;
using PortfolioContainerT = std::unordered_map<std::string, PortfolioPtrT>;

class StrategyWorkspace final {
  public:
    explicit StrategyWorkspace(boost::asio::io_context::strand& strand_);

    void Paint(bool* show_);

  protected:
    void DrawWindow(bool* show_);

    void DrawAddNewWorkspace();

    void Exports(const std::string& path_);

    void Imports(const std::string& path_);

  private:
    boost::asio::io_context::strand& _strand;

    PortfolioContainerT _portfolioContainer;

    bool _newWorkspace = false;

    std::string _strategyWorkspaceName;
    std::string _deleteWorkspace;
    std::string _fileName;
    std::string _strategyListIndex;
};
