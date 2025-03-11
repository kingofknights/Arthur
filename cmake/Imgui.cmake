include(FetchContent)

FetchContent_Declare(
  imGui URL https://github.com/ocornut/imgui/archive/refs/heads/docking.zip)

FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui)

message(STATUS "Imgui : ${imgui_SOURCE_DIR}")

set(BACKEND ${imgui_SOURCE_DIR}/backends)

set(IMGUI_BACKEND
    ${BACKEND}/imgui_impl_opengl3.h ${BACKEND}/imgui_impl_opengl3.cpp
    ${BACKEND}/imgui_impl_glfw.h ${BACKEND}/imgui_impl_glfw.cpp)

add_library(
  Imgui SHARED
  ${imgui_SOURCE_DIR}/imconfig.h
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui.h
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_internal.h
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imstb_rectpack.h
  ${imgui_SOURCE_DIR}/imstb_textedit.h
  ${imgui_SOURCE_DIR}/imstb_truetype.h
  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
  ${IMGUI_BACKEND})

target_include_directories(Imgui PRIVATE ${imgui_SOURCE_DIR})
set_target_properties(Imgui PROPERTIES C_CPPCHECK "" CXX_CPPCHECK "")
