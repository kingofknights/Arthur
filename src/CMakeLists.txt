include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Lancelot)

add_subdirectory(DataFeed)
add_subdirectory(API)
add_subdirectory(Knight)
# add_subdirectory(Excel) add_subdirectory(Audio)

set(HEADER_FILES
    ./database/Postgres.hpp
    ./include/BaseSocket.hpp
    ./include/Colors.hpp
    ./include/ConfigLoader.hpp
    ./include/Enums.hpp
    ./include/MarketWatch.hpp
    ./include/OpenOrders.hpp
    ./include/PortfolioInterface.hpp
    ./include/StrategyWorkspace.hpp
    ./include/TableColumnInfo.hpp
    ./include/Arthur.hpp
    ./include/TemplateBuilder.hpp
    ./include/Configuration.hpp
    ./include/OrderForm.hpp
    ./include/OrderHistory.hpp
    ./include/Portfolio.hpp
    ./include/Structure.hpp
    ./include/TradeHistory.hpp
    ./include/Multicast.hpp
    ./include/Login.hpp
    ./include/Utils.hpp
    ./include/MessageBroker.hpp
    ./include/OptionChain.hpp
    ./include/Position.hpp
    ./include/SplashScreen.hpp
    ./include/PortfolioScanner.hpp
    ./include/TradeTracker.hpp
    ./include/plf_nanotimer.h
    ./include/OrderBook.hpp
    ./include/Themes.hpp)

set(SOURCE_FILES
    ./database/Postgres.cpp
    ./source/TemplateBuilder.cpp
    ./source/OrderForm.cpp
    ./source/OrderHistory.cpp
    ./source/Portfolio.cpp
    ./source/TradeHistory.cpp
    ./source/ConfigLoader.cpp
    ./source/MarketWatch.cpp
    ./source/OpenOrders.cpp
    ./source/PortfolioInterface.cpp
    ./source/StrategyWorkspace.cpp
    ./source/Arthur.cpp
    ./source/External.cpp
    ./source/Login.cpp
    ./source/Multicast.cpp
    ./source/BaseSocket.cpp
    ./source/Utils.cpp
    ./source/MessageBroker.cpp
    ./source/OptionChain.cpp
    ./source/Position.cpp
    ./source/SplashScreen.cpp
    ./source/PortfolioScanner.cpp
    ./source/TradeTracker.cpp
    ./source/OrderBook.cpp
    ./source/Themes.cpp)

set(LANCELOT_FILES
    ./Lancelot/Logger.hpp
    ./Lancelot/StoreProcedures.hpp
    ./Lancelot/ContractFetcher.hpp
    ./Lancelot/ContractInfo.hpp
    ./Lancelot/Structure.hpp
    ./Lancelot/Enums.hpp
    ./Lancelot/Lancelot.hpp
    ./Lancelot/ContractFetcher.cpp
    ./Lancelot/ContractInfo.cpp
    ./Lancelot/Enums.cpp)

add_executable(${PROJECT_NAME} main_sdl3.cpp ${APP_ICON_RESOURCE_WINDOWS})

add_dependencies(${PROJECT_NAME} Imgui)
add_dependencies(${PROJECT_NAME} ImguiFD)

target_sources(${PROJECT_NAME} PRIVATE ${HEADER_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${LANCELOT_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE API)
target_link_libraries(${PROJECT_NAME} PRIVATE DataFeed)
target_link_libraries(${PROJECT_NAME} PRIVATE Knight)
target_link_libraries(${PROJECT_NAME} PRIVATE Greeks)

target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${imgui_SOURCE_DIR}/backends)

target_link_libraries(${PROJECT_NAME} PRIVATE Imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE ImguiFD)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLiteCpp)
target_link_libraries(${PROJECT_NAME} PRIVATE libpqxx::pqxx)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PRECOMPILE_HEADERS})

if(WIN32)
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_FILE} $<TARGET_FILE_DIR:${PROJECT_NAME}>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/setting.json
          $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_executable(buildResultSet ./buildResult.cpp)
target_sources(buildResultSet PRIVATE ${LANCELOT_FILES})
target_link_libraries(buildResultSet PRIVATE fmt::fmt)
target_link_libraries(buildResultSet PRIVATE SQLiteCpp)
