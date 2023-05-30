# This is an example of how to use FetchContent to easily
# manage 3rd-party dependencies
# First, check out src/CMakeLists.txt - it's really simple!
# Then, check out dependencies directory, it get a bit more complex
# there, but you'll see how it goes
#
# Options:
# * USE_SYSTEM_DEPS - try to find dependencies using find_package (OFF by default)
# * LINK_DEPS_STATIC - link to dependencies statically (ON by default)
#
cmake_minimum_required(VERSION 3.14)

project(TradingApp
	    VERSION 1.0
		LANGUAGES C CXX
	    )


set(CMAKE_CXX_CPPCHECK "")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

option(WEB "Make build for web browser" OFF)

if (WEB)
	set(CMAKE_EXECUTABLE_SUFFIX ".html")
	add_compile_options(-fno-stack-protector)
endif ()


if (WIN32)
	add_compile_definitions(WIN32_LEAN_AND_MEAN)
	add_compile_definitions(WINVER=0x0A00)
	add_compile_definitions(_WIN32_WINNT=0x0A00)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
	add_compile_options(/bigobj)
	include_directories($ENV{MSVC_INCLUDE_PATH})

endif ()

add_subdirectory(third_party)

include_directories(third_party/imgui)
include_directories(third_party/imgui/backends)
include_directories(third_party/ImGuiFileDialog)
include_directories(third_party/IconFontCppHeaders)
include_directories(third_party/xcd)
include_directories(third_party/lzo/include)
include_directories(third_party/stb)
include_directories(third_party/SDL-main/include)

set(FONT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Font/Ruda-Bold.ttf
	    ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Font/MaterialIcons-Regular.ttf
		${CMAKE_CURRENT_SOURCE_DIR}/Resources/collide.wav
	    )

set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/Resources/app.rc)
add_subdirectory(src)

add_custom_target(doc SOURCES README.md Features/FEATURES.md emscipten.bat .gitignore)
