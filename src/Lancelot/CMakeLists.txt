project(
  Lancelot
  VERSION 1.0
  LANGUAGES C CXX)

find_package(SQLiteCpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_library(
  ${PROJECT_NAME}
  Logger.hpp
  StoreProcedures.hpp
  ContractFetcher.hpp
  ContractInfo.hpp
  Structure.hpp
  Enums.hpp
  Lancelot.hpp
  ContractFetcher.cpp
  ContractInfo.cpp
  Enums.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only SQLiteCpp)
