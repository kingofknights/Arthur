set(SOURCE_FILES
        exprtk.hpp
        MathParser.hpp
        Scanner.hpp
        MathParser.cpp
        Scanner.cpp
        )
add_library(Knight ${SOURCE_FILES})

target_link_libraries(Knight PRIVATE API)
target_link_libraries(Knight PUBLIC nlohmann_json::nlohmann_json fmt::fmt)

target_precompile_headers(Knight
        PRIVATE
        <algorithm>
        <array>
        <chrono>
        <cstdint>
        <filesystem>
        <fstream>
        <iostream>
        <map>
        <memory>
        <ranges>
        <set>
        <sstream>
        <string>
        <string_view>
        <typeinfo>
        <unordered_map>
        <unordered_set>
        <variant>
        <vector>
        <thread>
        <memory>
        <nlohmann/json.hpp>
        "exprtk.hpp")
