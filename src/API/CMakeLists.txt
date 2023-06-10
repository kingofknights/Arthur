set(HEADER_FILES
        BaseScanner.hpp
        BaseScanner.cpp
        ContractInfo.hpp
        ContractInfo.cpp
        TokenInfo.hpp
        TokenInfo.cpp
        Common.hpp
        )

add_library(API ${HEADER_FILES})

target_link_libraries(API PUBLIC
        nlohmann_json::nlohmann_json
        fmt::fmt
        Boost::headers
        Boost::filesystem
        )

install(FILES
        BaseScanner.hpp
        ContractInfo.hpp
        TokenInfo.hpp
        Common.hpp
        DESTINATION
        ${EXECUTABLE_OUTPUT_PATH}/API/
        )
