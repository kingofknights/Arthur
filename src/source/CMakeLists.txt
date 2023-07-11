set(SOURCE_FILE
	    TemplateBuilder.cpp
		OrderForm.cpp
		OrderHistory.cpp
		Portfolio.cpp
		TradeHistory.cpp
		ConfigLoader.cpp
		MarketWatch.cpp
		OpenOrders.cpp
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
		Position.cpp
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
	Greeks
	repo
)
