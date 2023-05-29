#pragma once

#include <boost/asio.hpp>

#include "Structure.hpp"

class Portfolio;

using PortfolioPtrT		  = std::shared_ptr<Portfolio>;
using PortfolioContainerT = std::unordered_map<std::string, PortfolioPtrT>;

class StrategyWorkspace {
public:
	StrategyWorkspace(boost::asio::io_context::strand &strand_);
	void paint(bool *show_);

private:
	void DrawWindow(bool *show_);
	void DrawAddNewWorkspace();
	void Exports(std::string_view path_);
	void Imports(std::string_view path_);

private:
	char _strategyWorkspaceName[STRATEGY_NAME_LENGTH]{};

	PortfolioContainerT _portfolioContainer;
	std::string			_deleteWorkspace;
	std::string			_fileName;
	std::string			_strategyListIndex;
	bool				_newWorkspace = false;

	boost::asio::io_context::strand &_strand;
};
