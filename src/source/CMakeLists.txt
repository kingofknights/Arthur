set(SOURCE_FILE
        ColumnGenerator.cpp
        ManualOrder.cpp
        OrderHistory.cpp
        Portfolio.cpp
        Sqlite.cpp
        TradeBook.cpp
        ConfigLoader.cpp
        MarketWatch.cpp
        PendingBook.cpp
        PortfolioInterface.cpp
        StrategyWorkspace.cpp
        Arthur.cpp
        External.cpp
        Multicast.cpp
        BaseSocket.cpp
        Utils.cpp
        MessageBroker.cpp
        OptionChain.cpp
        #Excel.cpp
        GreekBook.cpp
        SplashScreen.cpp
        PortfolioScanner.cpp
        TradeTracker.cpp
        OrderBook.cpp
        Themes.cpp
        )

add_library(source ${SOURCE_FILE})
target_link_libraries(source PUBLIC
        nlohmann_json::nlohmann_json
        fmt::fmt
        SQLiteCpp
        )
