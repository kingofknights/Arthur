//
// Created by VIKLOD on 10-04-2023.
//

#include "ExcelAutomation.hpp"

#include <comdef.h>
#include <comutil.h>

#include "../include/Enums.hpp"
#include "../include/Logger.hpp"
#include "../include/TableColumnInfo.hpp"

#define EXCEL_TABLE_HEIGHT 14
#define EXCEL_CELL_HORIZONTAL_ALIGNMENT -4108

void __stdcall CustomRaiseError(HRESULT hr, IErrorInfo* pInfo) {
    if (pInfo) {
        // Get error description
        BSTR bstrDescription = NULL;
        pInfo->GetDescription(&bstrDescription);
        if (bstrDescription != NULL) {
            // Print error description to console
            _bstr_t bstr(bstrDescription, false);
            LOG(ERROR, "Error From Excel: {}", (const char*)bstr)
        }

        // Release error information
        pInfo->Release();
    }
}

double MyFunction(double arg1, double arg2) {
    LOG(ERROR, "{} {} {}", __FUNCTION__, arg1, arg2)
    return arg1 + arg2;
}

ExcelAutomationPtrT ExcelAutomation::_ptr;

ExcelAutomationPtrT& ExcelAutomation::Instance() {
    if (not _ptr) {
        _ptr = std::make_unique<ExcelAutomation>();
    }
    return _ptr;
}

ExcelAutomation::ExcelAutomation() {
    CoInitialize(nullptr);
    _set_com_error_handler(CustomRaiseError);

    HRESULT result = _application.CreateInstance(__uuidof(Excel::Application));
    LOG(INFO, "{}", "Trying to connect to Excel")

    if (FAILED(result)) {
        ShowErrorMessage("Unable to connect with Excel. Excel unavailable...");
        return;
    }
}

ExcelAutomation::~ExcelAutomation() {
    _application->Quit();
    CoUninitialize();
}

void ExcelAutomation::SetExcelVisibility(bool show_) { _application->Visible[0] = show_ ? VARIANT_TRUE : VARIANT_FALSE; }

Excel::_WorksheetPtr ExcelAutomation::AddSheets(std::string_view name_) {
    LOG(INFO, "ExcelAutomation::AddSheets {}", name_)
    Excel::_WorksheetPtr Sheets = _application->Workbooks->Add(Excel::xlWorksheet)->ActiveSheet;
    Sheets->Name                = _bstr_t(name_.data());
    return Sheets;
}

void ExcelAutomation::ShowErrorMessage(const std::string& message_) {
    // MessageBoxW(HWND_TOP, L(message_.data()), L"Error", MB_TOPMOST | MB_ICONERROR);
}

void ExcelAutomation::Save(ExcelSheetItemT& excelSheetItem_, std::filesystem::path path_) {
    LOG(INFO, "ExcelAutomation::Save {} {}", excelSheetItem_.Name, path_.string())
    std::string name = path_.append(excelSheetItem_.Name).string();
    LOG(INFO, "Removing File {}", name.append(".xlsx").data())
    std::remove(name.append(".xlsx").data());
    excelSheetItem_.Sheets->SaveAs(_bstr_t(name.data()), Excel::xlOpenXMLWorkbook);
    Excel::_WorkbookPtr workbook = excelSheetItem_.Sheets->Parent;
    workbook->Close(false);
    // excelSheetItem_.Sheets->Release();
}

Excel::_WorksheetPtr ExcelAutomation::OpenSheets(std::string_view path_, std::string_view name_) {
    LOG(INFO, "ExcelAutomation::OpenSheets {} {}", path_, name_)
    Excel::_WorkbookPtr  workbook = _application->Workbooks->Open(_bstr_t(path_.data()));
    Excel::_WorksheetPtr Sheets   = workbook->ActiveSheet;
    Sheets->Name                  = _bstr_t(name_.data());
    return Sheets;
}

void ExcelAutomation::SetValue(int index_, Excel::_WorksheetPtr sheet_, std::string_view name_) {
    std::string     rangeValue = FORMAT("A{}:F{}", 1 + index_ * EXCEL_TABLE_HEIGHT, 1 + index_ * EXCEL_TABLE_HEIGHT);
    Excel::RangePtr range      = sheet_->Range[_bstr_t(rangeValue.data())];
    range->Merge();
    range->Value2              = _bstr_t(name_.data());
    range->HorizontalAlignment = EXCEL_CELL_HORIZONTAL_ALIGNMENT;

    for (int i = 0; i < 6; ++i) {
        sheet_->Cells->Item[2 + index_ * EXCEL_TABLE_HEIGHT][i + 1]  = _bstr_t(MarketWatchTableToolTipColumnName[i]);
        sheet_->Cells->Item[8 + index_ * EXCEL_TABLE_HEIGHT][i + 1]  = _bstr_t(ExcelTableColumnName_1[i]);
        sheet_->Cells->Item[10 + index_ * EXCEL_TABLE_HEIGHT][i + 1] = _bstr_t(ExcelTableColumnName_2[i]);
    }
}

void ExcelAutomation::UpdateValue(int index_, Excel::_WorksheetPtr sheet_, const MarketWatchDataPtrT& marketWatchData_) {
    for (int i = 0; i < 5; ++i) {
        sheet_->Cells->Item[i + 3 + index_ * EXCEL_TABLE_HEIGHT][ExcelDataColumn_BID_ORDER]    = marketWatchData_->Bid[i].Order;
        sheet_->Cells->Item[i + 3 + index_ * EXCEL_TABLE_HEIGHT][ExcelDataColumn_BID_QUANTITY] = marketWatchData_->Bid[i].Quantity;
        sheet_->Cells->Item[i + 3 + index_ * EXCEL_TABLE_HEIGHT][ExcelDataColumn_BID_PRICE]    = marketWatchData_->Bid[i].Price;
        sheet_->Cells->Item[i + 3 + index_ * EXCEL_TABLE_HEIGHT][ExcelDataColumn_ASK_ORDER]    = marketWatchData_->Ask[i].Price;
        sheet_->Cells->Item[i + 3 + index_ * EXCEL_TABLE_HEIGHT][ExcelDataColumn_ASK_QUANTITY] = marketWatchData_->Ask[i].Quantity;
        sheet_->Cells->Item[i + 3 + index_ * EXCEL_TABLE_HEIGHT][ExcelDataColumn_ASK_PRICE]    = marketWatchData_->Ask[i].Order;
    }

    sheet_->Cells->Item[9 + index_ * EXCEL_TABLE_HEIGHT][ExcelSecondColumn_LTP]   = marketWatchData_->LastTradePrice;
    sheet_->Cells->Item[9 + index_ * EXCEL_TABLE_HEIGHT][ExcelSecondColumn_ATP]   = marketWatchData_->AverageTradePrice;
    sheet_->Cells->Item[9 + index_ * EXCEL_TABLE_HEIGHT][ExcelSecondColumn_OPEN]  = marketWatchData_->OpenPrice;
    sheet_->Cells->Item[9 + index_ * EXCEL_TABLE_HEIGHT][ExcelSecondColumn_HIGH]  = marketWatchData_->HighPrice;
    sheet_->Cells->Item[9 + index_ * EXCEL_TABLE_HEIGHT][ExcelSecondColumn_LOW]   = marketWatchData_->LowPrice;
    sheet_->Cells->Item[9 + index_ * EXCEL_TABLE_HEIGHT][ExcelSecondColumn_CLOSE] = marketWatchData_->ClosePrice;

    sheet_->Cells->Item[11 + index_ * EXCEL_TABLE_HEIGHT][ExcelThirdColumn_LOW_DPR]             = marketWatchData_->LowDPR;
    sheet_->Cells->Item[11 + index_ * EXCEL_TABLE_HEIGHT][ExcelThirdColumn_HIGH_DPR]            = marketWatchData_->HighDPR;
    sheet_->Cells->Item[11 + index_ * EXCEL_TABLE_HEIGHT][ExcelThirdColumn_TOTAL_BUY_QUANTITY]  = marketWatchData_->TotalBuyQuantity;
    sheet_->Cells->Item[11 + index_ * EXCEL_TABLE_HEIGHT][ExcelThirdColumn_TOTAL_SELL_QUANTITY] = marketWatchData_->TotalSellQuantity;
    sheet_->Cells->Item[11 + index_ * EXCEL_TABLE_HEIGHT][ExcelThirdColumn_VOLUME_TRADED_TODAY] = marketWatchData_->VolumeTradedToday;
    sheet_->Cells->Item[11 + index_ * EXCEL_TABLE_HEIGHT][ExcelThirdColumn_OPEN_INTEREST]       = marketWatchData_->OpenInterest;
}
