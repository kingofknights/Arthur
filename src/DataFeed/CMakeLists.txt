add_library(DataFeed STATIC ./CentralFeed.hpp ./CentralFeed.cpp)
target_precompile_headers(DataFeed PUBLIC ${PRECOMPILE_HEADERS})
