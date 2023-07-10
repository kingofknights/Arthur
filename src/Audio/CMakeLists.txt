add_library(Audio Sound.cpp Sound.hpp)
target_link_libraries(Audio PRIVATE SDL3 fmt::fmt)
target_compile_options(Audio PRIVATE -fpermissive)