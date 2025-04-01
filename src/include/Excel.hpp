//
// Created by VIKLOD on 10-04-2023.
//
#pragma once
#include "Structure.hpp"

class ExcelWindow {
  public:
    ExcelWindow();
    ~ExcelWindow();
    void paint(bool* show_);
    void Update(int token_);

  protected:
    void DrawExcelWindow(bool* show_);
    void Imports(std::string_view path_);
    void Exports(std::string_view path_);

  private:
    char        _sheetName[STRATEGY_NAME_LENGTH]{};
    bool        _showExcelApplication = false;
    std::string _contract;
};
