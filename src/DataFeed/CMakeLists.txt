add_library(DataFeed
        NeatData.cpp
        NeatData.hpp
        NeatStructure.hpp
        Compression.cpp
        Compression.h)


target_link_libraries(DataFeed
        PRIVATE
        fmt::fmt
        nlohmann_json::nlohmann_json
        lzo_static_lib
        XCD
        )

target_include_directories(DataFeed PRIVATE ${CMAKE_SOURCE_DIR}/third_party/lzo/include)
