add_library(Demo Demo.cpp Demo.hpp)
target_include_directories(Demo PUBLIC ${Boost_INCLUDE_DIRS})

#add_executable(Sound )
#target_link_libraries(Sound PRIVATE SDL3-shared)
