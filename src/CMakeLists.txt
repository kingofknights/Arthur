set(Boost_NO_WARN_NEW_VERSIONS OFF)

find_package(Boost REQUIRED COMPONENTS system thread filesystem)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

message(STATUS "Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIRS}")

find_package(Threads)
find_package(OpenGL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_subdirectory(include)
add_subdirectory(source)
add_subdirectory(DataFeed)
# add_subdirectory(Excel)
add_subdirectory(Knight)
add_subdirectory(API)
add_subdirectory(Demo)
add_subdirectory(Audio)

add_executable(${PROJECT_NAME} main.cpp ${APP_ICON_RESOURCE_WINDOWS})
add_dependencies(${PROJECT_NAME} SDL3-static)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE include
          source
          API
          DataFeed
          Knight
          Greeks
          # Excel
          Demo
          Audio)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC ImGui
         ImGuiFileDialog
         SDL3-static
         OpenGL::GL
         Threads::Threads)

target_link_directories(${PROJECT_NAME} PRIVATE ${Boost_LIBRARY_DIRS})

target_precompile_headers(
  ${PROJECT_NAME}
  PRIVATE
  <algorithm>
  <array>
  <chrono>
  <cstdint>
  <filesystem>
  <fstream>
  <iostream>
  <map>
  <memory>
  <ranges>
  <set>
  <sstream>
  <string>
  <string_view>
  <typeinfo>
  <unordered_map>
  <unordered_set>
  <variant>
  <vector>
  <thread>
  <memory>)

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_FILE} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
