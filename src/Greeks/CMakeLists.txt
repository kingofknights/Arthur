add_subdirectory(IVCalculator)
add_library(Greeks Greeks.cpp Greeks.hpp)
target_link_libraries(Greeks IV)