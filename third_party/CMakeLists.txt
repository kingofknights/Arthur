add_subdirectory(lzo)
if(NOT USE_SYSTEM_SDL3)
  add_subdirectory(SDL-main)
endif()

if(NOT USE_SYSTEM_SDL3)
  include_directories(SDL-main/include)
endif()

include_directories(ImGuiFileDialog)

set(IMGUI_FILEDIALOG_IMPLEMENTAION ImGuiFileDialog/ImGuiFileDialog.cpp ImGuiFileDialog/ImGuiFileDialog.h
                                   ImGuiFileDialog/ImGuiFileDialogConfig.h)

add_library(ImGuiFileDialog STATIC ${IMGUI_FILEDIALOG_IMPLEMENTAION})
