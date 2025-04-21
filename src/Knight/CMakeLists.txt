set(SOURCE_FILES
    exprtk.hpp
    MathParser.hpp
    Scanner.hpp
    MathParser.cpp
    Scanner.cpp)

add_library(Knight ${SOURCE_FILES})

target_link_libraries(Knight PRIVATE API)
target_link_libraries(Knight PUBLIC nlohmann_json::nlohmann_json fmt::fmt)

target_precompile_headers(Knight PRIVATE "exprtk.hpp")
