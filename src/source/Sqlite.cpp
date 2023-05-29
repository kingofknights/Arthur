#include "../include/Sqlite.hpp"

#include <future>

#include "../API/ContractInfo.hpp"
#include "../include/Logger.hpp"
#include "../include/StoreProcedures.hpp"
#include "../include/Utils.hpp"

static std::unique_ptr<Sqlite> instance = nullptr;
std::unique_ptr<Sqlite>&	   Sqlite::Instance() { return instance; }

extern AllContractT				AllContract;
extern NameToTokenContainerT	NameToTokenContainer;
extern ResultSetContainerT		ResultSetContainer;
extern TokenToFutureTokenT		TokenToFutureToken;
extern MarketWatchDatContainerT MarketWatchDatContainer;

void CreateMarketObject(uint32_t token_, std::string_view name_, float ltp_, float low_, float high_) {
	auto marketData	  = std::make_shared<MarketWatchDataT>();
	marketData->Token = token_;

	std::memset(marketData->Description.data(), '\0', STRATEGY_NAME_LENGTH);
	std::memcpy(marketData->Description.data(), name_.data(), name_.length());
	marketData->LastTradePrice = ltp_;
	marketData->LowPrice	   = low_;
	marketData->HighPrice	   = high_;
	marketData->ClosePrice	   = ltp_;
	MarketWatchDatContainer.emplace(token_, marketData);
}

void Sqlite::Init(std::string_view databaseName_) { instance = std::make_unique<Sqlite>(databaseName_); }

Sqlite::Sqlite(std::string_view databaseName_) : _database(databaseName_, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE), _query(_database, "Select * from ResultSet") {
	try {
		printMetaData(databaseName_.data());
		auto future = std::async(std::launch::async, [this]() { GetAllFutureToken(); });
		LoadContract();
		future.wait();
	} catch (std::exception& ex_) {
		LOG(ERROR, "{}", ex_.what())
	}
}

void Sqlite::printMetaData(const std::string& name_) {
	const SQLite::Header header = SQLite::Database::getHeaderInfo(name_);
	LOG(INFO, "SQLite Metadata -> Magic header string: {}", header.headerStr);
	LOG(INFO, "SQLite Metadata -> Page size bytes: {}", header.pageSizeBytes);
	LOG(INFO, "SQLite Metadata -> File format write version: {}", (int)header.fileFormatWriteVersion);
	LOG(INFO, "SQLite Metadata -> File format read version: {}", (int)header.fileFormatReadVersion);
	LOG(INFO, "SQLite Metadata -> Reserved space bytes: {}", (int)header.reservedSpaceBytes);
	LOG(INFO, "SQLite Metadata -> Max embedded payload fraction {}", (int)header.maxEmbeddedPayloadFrac);
	LOG(INFO, "SQLite Metadata -> Min embedded payload fraction: {}", (int)header.minEmbeddedPayloadFrac);
	LOG(INFO, "SQLite Metadata -> Leaf payload fraction: {}", (int)header.leafPayloadFrac);
	LOG(INFO, "SQLite Metadata -> File change counter: {}", header.fileChangeCounter);
	LOG(INFO, "SQLite Metadata -> Database size pages: {}", header.databaseSizePages);
	LOG(INFO, "SQLite Metadata -> First freelist trunk page: {}", header.firstFreelistTrunkPage);
	LOG(INFO, "SQLite Metadata -> Total freelist trunk pages: {}", header.totalFreelistPages);
	LOG(INFO, "SQLite Metadata -> Schema cookie: {}", header.schemaCookie);
	LOG(INFO, "SQLite Metadata -> Schema format number: {}", header.schemaFormatNumber);
	LOG(INFO, "SQLite Metadata -> Default page cache size bytes: {}", header.defaultPageCacheSizeBytes);
	LOG(INFO, "SQLite Metadata -> Largest B tree page number: {}", header.largestBTreePageNumber);
	LOG(INFO, "SQLite Metadata -> Database text encoding: {}", header.databaseTextEncoding);
	LOG(INFO, "SQLite Metadata -> User version: {}", header.userVersion);
	LOG(INFO, "SQLite Metadata -> Incremental vaccum mode: {}", header.incrementalVaccumMode);
	LOG(INFO, "SQLite Metadata -> Application ID: {}", header.applicationId);
	LOG(INFO, "SQLite Metadata -> Version valid for: {}", header.versionValidFor);
	LOG(INFO, "SQLite Metadata -> SQLite version: {}", header.sqliteVersion);
}

DatabaseTable Sqlite::GetResult(const std::string& query_) {
	DatabaseTable	  table;
	SQLite::Statement query(_database, query_);
	while (query.executeStep()) {
		DatabaseRow row;
		for (int i = 0; i < query.getColumnCount(); i++) {
			row.push_back(query.getColumn(i).getString());
		}
		table.push_back(row);
	}
	return table;
}
void Sqlite::LoadContract() {
	while (_query.executeStep()) {
		ResultSetT resultSet;

		resultSet.Segment	  = _query.getColumn("Segment").getString();
		resultSet.Token		  = _query.getColumn("Token").getInt();
		resultSet.Symbol	  = _query.getColumn("Symbol").getString();
		resultSet.ExpiryDate  = _query.getColumn("ExpiryDate").getInt();
		resultSet.InstType	  = _query.getColumn("InstType").getString();
		resultSet.OptionType  = _query.getColumn("OptionType").getString();
		resultSet.LotMultiple = _query.getColumn("LotMultiple").getInt();
		resultSet.LotSize	  = _query.getColumn("LotSize").getInt();
		resultSet.TickSize	  = _query.getColumn("TickSize").getInt();
		resultSet.Name		  = _query.getColumn("Name").getString();
		resultSet.series	  = _query.getColumn("series").getString();
		resultSet.Divisor	  = _query.getColumn("Divisor").getInt();
		resultSet.Exchange	  = _query.getColumn("Exchange").getString();
		// resultSet.FullName	  = _query.getColumn("FullName").getString();
		resultSet.StrikePrice = _query.getColumn("StrikePrice").getDouble() / resultSet.Divisor;

		std::stringstream ss;
		ss << (resultSet.StrikePrice < 0 ? "FUT" : "OPT");
		ss << ' ' << resultSet.Symbol;
		if (resultSet.StrikePrice > 0) {
			ss << ' ' << (resultSet.StrikePrice) << ' ' << resultSet.OptionType;
		}
		ss << ' ' << fmt::format("{:%d%b}", fmt::localtime(resultSet.ExpiryDate));
		resultSet.FullName = ss.str();
		ResultSetContainer.emplace(resultSet.Token, resultSet);
		NameToTokenContainer.emplace(resultSet.FullName, resultSet.Token);

		CreateMarketObject(resultSet.Token, resultSet.FullName, _query.getColumn("Close").getDouble() / resultSet.Divisor, _query.getColumn("LowDPR").getDouble() / resultSet.Divisor,
						   _query.getColumn("HighDPR").getDouble() / resultSet.Divisor);
		AllContract.push_back(resultSet.FullName);
	}
	LOG(INFO, "{} {}", __FUNCTION__, AllContract.size());
	std::sort(AllContract.begin(), AllContract.end(), std::less<>());
}

void Sqlite::GetAllFutureToken() {
	enum {
		OPTION_TOKEN = 0,
		FUTURE_TOKEN
	};
	SQLite::Statement query(_database, GetFuture_);
	while (query.executeStep()) {
		TokenToFutureToken.emplace(query.getColumn(OPTION_TOKEN).getInt(), query.getColumn(FUTURE_TOKEN).getInt());
	}
	LOG(INFO, "{} {}", __FUNCTION__, TokenToFutureToken.size());
}
void Sqlite::ExecuteQuery(const std::string& query_) {
	try {
		SQLite::Statement query(_database, query_);
		query.executeStep();
	} catch (std::exception& ex_) {
		LOG(ERROR, "{} {}", __FUNCTION__, ex_.what())
	}
}
