add_subdirectory(Greeks)
# add_subdirectory(xcd)

add_library(repo INTERFACE)

target_link_libraries(repo INTERFACE Greeks)
# target_link_libraries(repo INTERFACE xcd)
