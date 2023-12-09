cmake_minimum_required(VERSION 3.14)

project(Arthur VERSION 1.0 LANGUAGES C CXX)

include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
    if (ENABLE_BUILD_WITH_TIME_TRACE)
        add_compile_definitions(project_options INTERFACE -ftime-trace)
    endif ()
endif ()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# enable cache system
set(ENABLE_CACHE OFF)
include(cmake/Cache.cmake)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# enable doxygen
include(cmake/Doxygen.cmake)
enable_doxygen()

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF)

# Very basic PCH example
option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if (ENABLE_PCH)
    # This sets a global PCH parameter, each project will build its own PCH, which is a good idea if any #define's change
    #
    # consider breaking this out per project as necessary
    target_precompile_headers(
            project_options
            INTERFACE
            <vector>
            <string>
            <map>
            <utility>)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

if (WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    add_compile_definitions(WINVER=0x0A00)
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/bigobj)
    add_compile_options(/c)
    include_directories($ENV{MSVC_INCLUDE_PATH})
    include_directories($ENV{External_Header_PATH})
endif ()


set(USE_SYSTEM_SDL3 OFF)
message(STATUS "Using System SDL3 ${USE_SYSTEM_SDL3}")
add_subdirectory(repo)
add_subdirectory(third_party)

include_directories(third_party/imgui)
include_directories(third_party/imgui/backends)
include_directories(third_party/ImGuiFileDialog)
include_directories(third_party/IconFontCppHeaders)
include_directories(third_party/lzo/include)
include_directories(third_party/stb)

include_directories(repo/Lancelot/include)
include_directories(repo/Greeks)
include_directories(repo/xcd)

if (NOT USE_SYSTEM_SDL3)
    include_directories(third_party/SDL-main/include)
endif ()

set(FONT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Font/Ruda-Bold.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Font/MaterialIcons-Regular.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/collide.wav
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ResultSet.db3
)

set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/Resources/app.rc)

add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} INTERFACE project_options project_warnings)

