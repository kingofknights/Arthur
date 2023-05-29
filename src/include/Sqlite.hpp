#pragma once

#define SQLITE_ENABLE_COLUMN_METADATA
#include <SQLiteCpp/SQLiteCpp.h>

#include "PreCompileHeader.hpp"
#include "Structure.hpp"

class Sqlite {
public:
	explicit Sqlite(std::string_view databaseName_);

	static std::unique_ptr<Sqlite>& Instance();
	static void						Init(std::string_view databaseName_);
	void							ExecuteQuery(const std::string& query_);
	DatabaseTable					GetResult(const std::string& query_);

private:
	void printMetaData(const std::string& name_);
	void LoadContract();
	void GetAllFutureToken();

	SQLite::Database  _database;
	SQLite::Statement _query;
};
