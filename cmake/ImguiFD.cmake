include(FetchContent)

FetchContent_Declare(imgui_fd URL https://github.com/aiekick/ImGuiFileDialog/archive/refs/tags/v0.6.7.zip)

FetchContent_MakeAvailable(imgui_fd)
FetchContent_GetProperties(imgui_fd)

message(STATUS "Imgui FD : ${imgui_fd_SOURCE_DIR}")
include_directories(${imgui_fd_SOURCE_DIR})

set(IMGUI_FD_SOURCE ${imgui_fd_SOURCE_DIR}/ImGuiFileDialog.h ${imgui_fd_SOURCE_DIR}/ImGuiFileDialog.cpp
                    ${imgui_fd_SOURCE_DIR}/ImGuiFileDialogConfig.h)

add_library(ImguiFD STATIC ${IMGUI_FD_SOURCE})
add_dependencies(ImguiFD Imgui)

target_include_directories(ImguiFD PUBLIC ${imgui_SOURCE_DIR})

set_target_properties(ImguiFD PROPERTIES C_CPPCHECK "" CXX_CPPCHECK "")
