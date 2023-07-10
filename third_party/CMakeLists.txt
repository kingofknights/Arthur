
add_subdirectory(lzo)

#[[
include(FetchContent)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
  OVERRIDE_FIND_PACKAGE
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
	FetchContent_Populate(imgui)
	#add_subdirectory(${imgui_SOURCE_DIR} ${greeks_BINARY_DIR})
	include_directories(${imgui_SOURCE_DIR})
	message(STATUS "imgui_SOURCE_DIR" ${imgui_SOURCE_DIR})
	message(STATUS "imgui_BINARY_DIR" ${imgui_BINARY_DIR})
endif()

set(IMGUI_DIR ${imgui_SOURCE_DIR})
]]
set(IMGUI_DIR imgui)
set(BACKEND ${IMGUI_DIR}/backends)

include_directories(${IMGUI_DIR})
include_directories(${BACKEND})
include_directories(SDL-main/include)

include_directories(ImGuiFileDialog)

set(IMGUI_IMPLEMENTAION
	    ${IMGUI_DIR}/imconfig.h
		${IMGUI_DIR}/imgui.cpp
		${IMGUI_DIR}/imgui.h
		${IMGUI_DIR}/imgui_demo.cpp
		${IMGUI_DIR}/imgui_draw.cpp
		${IMGUI_DIR}/imgui_internal.h
		${IMGUI_DIR}/imgui_tables.cpp
		${IMGUI_DIR}/imgui_widgets.cpp
		${IMGUI_DIR}/imstb_rectpack.h
		${IMGUI_DIR}/imstb_textedit.h
		${IMGUI_DIR}/imstb_truetype.h
		${IMGUI_DIR}/misc/cpp/imgui_stdlib.h
		${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
	    )

set(IMGUI_BACKEND
	    ${BACKEND}/imgui_impl_opengl3.h
		${BACKEND}/imgui_impl_opengl3.cpp
		${BACKEND}/imgui_impl_sdl3.h
		${BACKEND}/imgui_impl_sdl3.cpp
	    )

set(IMGUI_FILEDIALOG_IMPLEMENTAION
	    ImGuiFileDialog/ImGuiFileDialog.cpp
		ImGuiFileDialog/ImGuiFileDialog.h
		ImGuiFileDialog/ImGuiFileDialogConfig.h)


add_library(ImGui ${IMGUI_IMPLEMENTAION} ${IMGUI_BACKEND})
add_library(ImGuiFileDialog ${IMGUI_FILEDIALOG_IMPLEMENTAION})

