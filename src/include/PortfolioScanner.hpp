//
// Created by VIKLOD on 24-04-2023.
//
#pragma once
#include "Structure.hpp"

#define SCANNER_WINDOW "Scanner Window"

class PortfolioScanner : public std::enable_shared_from_this<PortfolioScanner> {
  public:
    PortfolioScanner(const std::string& strategyName_);

    void paint(bool* show_);
    void Export(std::string_view path_);
    void Import(std::string_view path_);

  protected:
    void ScannerWindow(bool* show_);
    void LoadParameter();

    void FirstColumn();
    void SecondColumn();
    void ThirdColumn();
    void CreateFormula();

  protected:
    ParameterInfoListT _paramList;
    StrategyListT      _strategyList;

  private:
    ScannerFunctionListContainerT _scannerFunctionListContainer;
    ScannerInfoFromDatabaseT      _scannerInfoFromDatabase;
    ScannerSaveContainerT         _scannerSaveContainer;

    bool        _hasParameter;
    int         _selectedParam;
    int         _strategyID;
    int         _selectedScanner = -1;
    int         _deleteScannerID = -1;
    std::string _equations;
    std::string _formulaName;
    std::string _strategyName;
    std::string _unfoldedFormula;
};
