cmake_minimum_required(VERSION 3.14)

project(
  Arthur
  VERSION 1.0
  LANGUAGES C CXX)

include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

add_subdirectory(repo)

include_directories(third_party/IconFontCppHeaders)
include_directories(third_party/stb)

include(./cmake/Imgui.cmake)
include(./cmake/ImguiFD.cmake)

include_directories(repo/Lancelot/include)
include_directories(repo/Greeks)

set(FONT_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Font/Ruda-Bold.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Font/MaterialIcons-Regular.ttf
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/collide.wav
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ResultSet.db3)

set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/Resources/app.rc)

add_subdirectory(src)
