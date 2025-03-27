include(FetchContent)

FetchContent_Declare(glfw URL https://github.com/glfw/glfw/archive/refs/tags/3.4.zip)
FetchContent_MakeAvailable(glfw)
FetchContent_GetProperties(glfw)

message(STATUS "glfw_DIR : ${glfw_SOURCE_DIR}")

set(GLFW_LIBRARY_TYPE STATIC)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_WAYLAND OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_X11 ON)

include_directories(${glfw_SOURCE_DIR}/include)
