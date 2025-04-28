include(FetchContent)

FetchContent_Declare(imgui URL https://github.com/ocornut/imgui/archive/refs/heads/docking.zip)

FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui)

message(STATUS "Imgui : ${imgui_SOURCE_DIR}")

include_directories(${imgui_SOURCE_DIR})
include_directories(${imgui_SOURCE_DIR}/misc/cpp)

set(BACKEND ${imgui_SOURCE_DIR}/backends)

set(IMGUI_BACKEND
    ${BACKEND}/imgui_impl_sdl3.h
    ${BACKEND}/imgui_impl_sdl3.cpp
    ${BACKEND}/imgui_impl_sdlrenderer3.h
    ${BACKEND}/imgui_impl_sdlrenderer3.cpp)

add_library(
  Imgui STATIC
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

target_link_libraries(Imgui PRIVATE SDL3::SDL3)

set_target_properties(Imgui PROPERTIES C_CPPCHECK "" CXX_CPPCHECK "")
