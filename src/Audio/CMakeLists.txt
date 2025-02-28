add_library(Audio Sound.cpp Sound.hpp)
target_link_libraries(Audio PRIVATE SDL3-shared fmt::fmt-header-only)
target_compile_options(Audio PRIVATE -fpermissive)
