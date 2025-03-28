include(FetchContent)

set(FMT_UNICODE 0)
FetchContent_Declare(fmt URL https://github.com/fmtlib/fmt/releases/download/11.1.4/fmt-11.1.4.zip)
FetchContent_MakeAvailable(fmt)
FetchContent_GetProperties(fmt)


message(STATUS "fmt_DIR : ${fmt_SOURCE_DIR}")
include_directories(${fmt_SOURCE_DIR}/include)
