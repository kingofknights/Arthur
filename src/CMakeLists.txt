set(Boost_NO_WARN_NEW_VERSIONS OFF)

find_package(Boost REQUIRED COMPONENTS system thread filesystem)
find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

message(STATUS "Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS ${Boost_LIBRARY_DIRS}")

add_subdirectory(include)
add_subdirectory(source)
add_subdirectory(DataFeed)
# add_subdirectory(Excel)
add_subdirectory(Knight)
add_subdirectory(API)
# add_subdirectory(Audio)

add_executable(${PROJECT_NAME} main.cpp ${APP_ICON_RESOURCE_WINDOWS})
add_dependencies(${PROJECT_NAME} Imgui ImguiFD)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE include
          source
          API
          DataFeed
          Knight
          Greeks
          # Excel
)

target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_IMPL_OPENGL_ES2)
target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR}/backends)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw GL)
target_link_libraries(${PROJECT_NAME} PRIVATE Imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE ImguiFD)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_gl_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_FILE} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
