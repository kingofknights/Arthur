#set(CMAKE_CXX_CPPCHECK "cppcheck")

set(Boost_NO_WARN_NEW_VERSIONS ON)
set(Boost_USE_STATIC_LIBS ON)

include(FetchContent)

find_package(Threads)
find_package(OpenGL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(SQLiteCpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)


FetchContent_Declare(
  Greeks
  GIT_REPOSITORY https://github.com/kingofknights/Greeks.git
  OVERRIDE_FIND_PACKAGE
)

FetchContent_GetProperties(Greeks)
if(NOT Greeks_POPULATED)
	FetchContent_Populate(Greeks)
	add_subdirectory(${greeks_SOURCE_DIR} ${greeks_BINARY_DIR})
	include_directories(${greeks_SOURCE_DIR})
	message(STATUS "Greeks_SOURCE_DIR" ${greeks_SOURCE_DIR})
	message(STATUS "Greeks_BINARY_DIR" ${greeks_BINARY_DIR})
endif()



add_subdirectory(include)
add_subdirectory(source)
add_subdirectory(DataFeed)
#add_subdirectory(Excel)
add_subdirectory(Knight)
add_subdirectory(API)
add_subdirectory(Database)
add_subdirectory(Demo)
add_subdirectory(Audio)

add_executable(${PROJECT_NAME}
	    main.cpp
		${APP_ICON_RESOURCE_WINDOWS})

target_link_libraries(${PROJECT_NAME}
	    PRIVATE
		include
		source
		API
		DataFeed
		Knight
		Greeks
		#Excel
		Demo
		Audio
	    )

target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
#target_compile_options(${PROJECT_NAME} PRIVATE -DIMGUI_IMPL_OPENGL_ES2)
target_link_libraries(${PROJECT_NAME}
	    PUBLIC
		ImGui
		ImGuiFileDialog
		SDL3-shared
		OpenGL::GL
		${BOOST_LIBRARIES}
		Threads::Threads
		#pugixml
		#pugixml::shared
		#pugixml::pugixml
	    )


target_precompile_headers(${PROJECT_NAME}
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
		<memory>
	    )








#[[
if (WIN32)
	target_compile_options(${PROJECT_NAME} PRIVATE /analyze)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		    COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:SDL2-shared>
			$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif ()
]]
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${FONT_FILE}
		$<TARGET_FILE_DIR:${PROJECT_NAME}>)
