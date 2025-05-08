include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Lancelot)

add_subdirectory(API)
#[[
add_subdirectory(Knight)
add_subdirectory(Excel)
add_subdirectory(Audio)
]]

set(HEADER_FILES
    # cmake-format: sortable
    ./database/Postgres.hpp
    ./DataFeed/CentralFeed.hpp
    ./include/Arthur.hpp
    ./include/BaseSocket.hpp
    ./include/Colors.hpp
    ./include/ConfigLoader.hpp
    ./include/Configuration.hpp
    ./include/Enums.hpp
    ./include/Login.hpp
    ./include/MarketWatch.hpp
    ./include/MessageBroker.hpp
    ./include/Multicast.hpp
    ./include/OpenOrders.hpp
    ./include/OptionChain.hpp
    ./include/OrderBook.hpp
    ./include/OrderForm.hpp
    ./include/OrderHistory.hpp
    ./include/plf_nanotimer.h
    ./include/Portfolio.hpp
    ./include/PortfolioInterface.hpp
    ./include/PortfolioScanner.hpp
    ./include/Position.hpp
    # ./include/SplashScreen.hpp
    ./include/StrategyWorkspace.hpp
    ./include/Structure.hpp
    ./include/TableColumnInfo.hpp
    ./include/TemplateBuilder.hpp
    ./include/Themes.hpp
    ./include/TradeHistory.hpp
    ./include/TradeTracker.hpp
    ./include/Utils.hpp)

set(SOURCE_FILES
    # cmake-format: sortable
    ./database/Postgres.cpp
    ./DataFeed/CentralFeed.cpp
    ./source/Arthur.cpp
    ./source/BaseSocket.cpp
    ./source/ConfigLoader.cpp
    ./source/External.cpp
    ./source/Login.cpp
    ./source/MarketWatch.cpp
    ./source/MessageBroker.cpp
    ./source/Multicast.cpp
    ./source/OpenOrders.cpp
    ./source/OptionChain.cpp
    ./source/OrderBook.cpp
    ./source/OrderForm.cpp
    ./source/OrderHistory.cpp
    ./source/Portfolio.cpp
    ./source/PortfolioInterface.cpp
    ./source/PortfolioScanner.cpp
    ./source/Position.cpp
    # ./source/SplashScreen.cpp
    ./source/StrategyWorkspace.cpp
    ./source/TemplateBuilder.cpp
    ./source/Themes.cpp
    ./source/TradeHistory.cpp
    ./source/TradeTracker.cpp
    ./source/Utils.cpp)

set(LANCELOT_FILES
    # cmake-format: sortable
    ./Lancelot/ContractFetcher.cpp
    ./Lancelot/ContractFetcher.hpp
    ./Lancelot/ContractInfo.cpp
    ./Lancelot/ContractInfo.hpp
    ./Lancelot/Enums.cpp
    ./Lancelot/Enums.hpp
    ./Lancelot/Lancelot.hpp
    ./Lancelot/Logger.hpp
    ./Lancelot/StoreProcedures.hpp
    ./Lancelot/Structure.hpp)

add_executable(${PROJECT_NAME} ${APP_ICON_RESOURCE_WINDOWS} main_sdl3.cpp)

add_dependencies(${PROJECT_NAME} Imgui)
add_dependencies(${PROJECT_NAME} ImguiFD)

target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${LANCELOT_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE API)
# target_link_libraries(${PROJECT_NAME} PRIVATE Knight)
target_link_libraries(${PROJECT_NAME} PRIVATE Greeks)

target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR}/backends)

target_link_libraries(${PROJECT_NAME} PRIVATE Imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE ImguiFD)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp)
target_link_libraries(${PROJECT_NAME} PRIVATE libpqxx::pqxx)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::headers)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PRECOMPILE_HEADERS})

if(WIN32 AND MSVC)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_FILE} $<TARGET_FILE_DIR:${PROJECT_NAME}>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/setting.json
          $<TARGET_FILE_DIR:${PROJECT_NAME}>
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${PROJECT_NAME}> $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
  COMMAND_EXPAND_LISTS)

add_executable(buildResultSet ./buildResult.cpp)
target_sources(buildResultSet PRIVATE ${LANCELOT_FILES})
target_link_libraries(buildResultSet PRIVATE fmt::fmt)
target_link_libraries(buildResultSet PRIVATE SQLiteCpp)
target_link_libraries(buildResultSet PRIVATE Boost::headers)
