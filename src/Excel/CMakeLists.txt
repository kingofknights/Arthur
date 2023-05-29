
set(EXCEL_FILES ExcelAutomation.cpp ExcelAutomation.hpp ExcelHeader.hpp)

add_library(Excel ${EXCEL_FILES})

target_link_libraries(Excel PUBLIC
        nlohmann_json::nlohmann_json
        fmt::fmt)
