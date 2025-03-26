include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
FetchContent_GetProperties(json)

message(STATUS "json_DIR : ${json_SOURCE_DIR}")
