include(FetchContent)

FetchContent_Declare(sqlite URL https://github.com/SRombauts/SQLiteCpp/archive/refs/tags/3.3.2.zip)
FetchContent_MakeAvailable(sqlite)
FetchContent_GetProperties(sqlite)

message(STATUS "SQLiteCpp_DIR : ${sqlite_SOURCE_DIR}")
include_directories(${sqlite_SOURCE_DIR}/include)