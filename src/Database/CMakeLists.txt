

set(MYSQL_INCLUDE_PATH "C:/Program Files/MySQL/Connector C++ 8.0/include")
set(MYSQL_LIB_PATH "C:/Program Files/MySQL/Connector C++ 8.0/lib64/vs14")
set(MYSQL_LIB "${MYSQL_LIB_PATH}/mysqlcppconn.lib")

set(FILES MySqlConnector.cpp MySqlConnector.hpp)
add_library(MySqlConnector ${FILES})
target_link_libraries(MySqlConnector PUBLIC ${MYSQL_LIB} fmt::fmt)
target_link_directories(MySqlConnector PUBLIC ${MYSQL_LIB_PATH})
target_include_directories(MySqlConnector PUBLIC ${MYSQL_INCLUDE_PATH})