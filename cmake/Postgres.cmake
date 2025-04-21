include(FetchContent)

FetchContent_Declare(pqxx URL https://github.com/jtv/libpqxx/archive/refs/tags/7.10.1.tar.gz)
FetchContent_MakeAvailable(pqxx)
FetchContent_GetProperties(pqxx)

message(STATUS "pqxx_DIR : ${pqxx_SOURCE_DIR}")
include_directories(${pqxx_SOURCE_DIR}/include)
