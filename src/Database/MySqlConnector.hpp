//
// Created by VIKLOD on 27-04-2023.
//
#pragma once

#include <mysqlx/xdevapi.h>

#include <string>

#include "../include/Structure.hpp"

class MySQLConnector {
public:
	MySQLConnector();
	virtual ~MySQLConnector();

	bool Connect(const std::string& url_, const std::string& user_, const std::string& password_, const std::string& database_);
	void SetSchema(std::string_view database_);

	DatabaseTable ExecuteQuery(std::string_view query_);

private:
	mysqlx::Session _session;
};
