add_library(DataFeed STATIC # cmake-format: sortable
            ./CentralFeed.cpp ./CentralFeed.hpp)
target_precompile_headers(DataFeed PUBLIC ${PRECOMPILE_HEADERS})
target_link_libraries(DataFeed PUBLIC Boost::headers)
