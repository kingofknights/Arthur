
set(HEADER_FILE
        BaseSocket.hpp
        Colors.hpp
        ConfigLoader.hpp
        Enums.hpp
        MarketWatch.hpp
        OpenOrders.hpp
        PortfolioInterface.hpp
        StrategyWorkspace.hpp
        TableColumnInfo.hpp
        Arthur.hpp
        TemplateBuilder.hpp
        Configuration.hpp
        OrderForm.hpp
        OrderHistory.hpp
        Portfolio.hpp
        PreCompileHeader.hpp
        Structure.hpp
        TradeHistory.hpp
        Multicast.hpp
        Utils.hpp
        MessageBroker.hpp
        OptionChain.hpp
        #Excel.hpp
        Position.hpp
        Signal.hpp
        SplashScreen.hpp
        PortfolioScanner.hpp
        TradeTracker.hpp
        plf_nanotimer.h
        OrderBook.hpp
        Themes.hpp
)

add_library(include INTERFACE ${HEADER_FILE})
set_target_properties(include PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(include INTERFACE nlohmann_json::nlohmann_json fmt::fmt)
