//
// Created by VIKLOD on 24-04-2023.
//
#pragma once
#include "Structure.hpp"

#define SCANNER_WINDOW "Scanner Window"

class PortfolioScanner : public std::enable_shared_from_this<PortfolioScanner> {
  public:
    explicit PortfolioScanner(std::string strategyName_);

    void Paint(bool* show_);

    void Export(const std::string& path_);

    void Import(const std::string& path_);

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
    const std::string             _strategyName;
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
    std::string _unfoldedFormula;
};
