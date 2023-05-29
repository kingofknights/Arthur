//
// Created by VIKLOD on 27-04-2023.
//

#include "MySqlConnector.hpp"

#include <iostream>

#include "../include/Logger.hpp"

MySQLConnector::MySQLConnector() : _session("mysqlx://root@127.0.0.1") {}
MySQLConnector::~MySQLConnector() {}

bool MySQLConnector::Connect(const std::string& url_, const std::string& user_, const std::string& password_, const std::string& database_) {
	auto schema = _session.getSchemas();
	for (const auto& item : schema) {
		std::cout << item.getName() << std::endl;
	}

	SetSchema(database_);
	return true;
}
void MySQLConnector::SetSchema(std::string_view database_) {}

DatabaseTable MySQLConnector::ExecuteQuery(std::string_view query_) { return {}; }
