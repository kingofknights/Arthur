add_library(DataFeed STATIC ./CentralFeed.hpp ./CentralFeed.cpp)
target_precompile_headers(DataFeed PUBLIC ${PRECOMPILE_HEADERS})
target_link_libraries(DataFeed PUBLIC Boost::headers)
