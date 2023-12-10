#pragma once

#include <string>

// clang-format off
#import "C:\Program Files (x86)\Microsoft Office\root\VFS\ProgramFilesCommonX86\Microsoft Shared\OFFICE16\mso.dll" rename("RGB", "MSRGB") rename("DocumentProperties", "WordDocumentProperties")
#import "C:\Program Files (x86)\Microsoft Office\root\VFS\ProgramFilesCommonX86\Microsoft Shared\VBA\VBA6\VBE6EXT.OLB" rename("Reference", "ignorethis") rename("VBE", "testVBE") raw_interfaces_only
#import "C:\Program Files (x86)\Microsoft Office\root\Office16\Excel.exe" exclude("IFont", "IPicture") rename("RGB", "ignorethis") rename("DialogBox", "ignorethis") rename("VBE", "testVBE") rename("ReplaceText", "EReplaceText") rename("CopyFile", "ECopyFile") rename("FindText", "EFindText") rename("NoPrompt", "ENoPrompt")
// clang-format on

#include "../include/Structure.hpp"
#include "imgui.h"

using ExcelSheetItemT = struct ExcelSheetItemT {
    int                        Index;
    ImGuiTextFilter            Filter;
    std::string                Name;
    Excel::_WorksheetPtr       Sheets;
    ExcelContactItemContainerT ExcelContactItemContainer;
};

using ExcelSheetItemContainerT = std::unordered_map<std::string, ExcelSheetItemT>;
