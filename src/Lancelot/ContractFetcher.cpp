#include "ContractFetcher.hpp"

#include "Enums.hpp"
#include "Logger.hpp"
#include "StoreProcedures.hpp"

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>

#include <algorithm>

namespace Lancelot {
    ContractFetcher::ContractFetcher(const std::string& name_)
        : _database(new SQLite::Database(name_, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)) {
        // PrintMetaData(name_);
    }

    ContractFetcher::~ContractFetcher() noexcept { delete _database; }

    void ContractFetcher::PrintMetaData(const std::string& name_) {
        const SQLite::Header header = SQLite::Database::getHeaderInfo(name_);
        LOG(INFO, "SQLite Metadata -> Magic header string: {}", header.headerStr)
        LOG(INFO, "SQLite Metadata -> Page size bytes: {}", header.pageSizeBytes)
        LOG(INFO, "SQLite Metadata -> File format write version: {}", (int)header.fileFormatWriteVersion)
        LOG(INFO, "SQLite Metadata -> File format read version: {}", (int)header.fileFormatReadVersion)
        LOG(INFO, "SQLite Metadata -> Reserved space bytes: {}", (int)header.reservedSpaceBytes)
        LOG(INFO, "SQLite Metadata -> Max embedded payload fraction {}", (int)header.maxEmbeddedPayloadFrac)
        LOG(INFO, "SQLite Metadata -> Min embedded payload fraction: {}", (int)header.minEmbeddedPayloadFrac)
        LOG(INFO, "SQLite Metadata -> Leaf payload fraction: {}", (int)header.leafPayloadFrac)
        LOG(INFO, "SQLite Metadata -> File change counter: {}", header.fileChangeCounter)
        LOG(INFO, "SQLite Metadata -> Database size pages: {}", header.databaseSizePages)
        LOG(INFO, "SQLite Metadata -> First freelist trunk page: {}", header.firstFreelistTrunkPage)
        LOG(INFO, "SQLite Metadata -> Total freelist trunk pages: {}", header.totalFreelistPages)
        LOG(INFO, "SQLite Metadata -> Schema cookie: {}", header.schemaCookie)
        LOG(INFO, "SQLite Metadata -> Schema format number: {}", header.schemaFormatNumber)
        LOG(INFO, "SQLite Metadata -> Default page cache size bytes: {}", header.defaultPageCacheSizeBytes)
        LOG(INFO, "SQLite Metadata -> Largest B tree page number: {}", header.largestBTreePageNumber)
        LOG(INFO, "SQLite Metadata -> Database text encoding: {}", header.databaseTextEncoding)
        LOG(INFO, "SQLite Metadata -> User version: {}", header.userVersion)
        LOG(INFO, "SQLite Metadata -> Incremental vaccum mode: {}", header.incrementalVaccumMode)
        LOG(INFO, "SQLite Metadata -> Application ID: {}", header.applicationId)
        LOG(INFO, "SQLite Metadata -> Version valid for: {}", header.versionValidFor)
        LOG(INFO, "SQLite Metadata -> SQLite version: {}", header.sqliteVersion)
    }

    void ContractFetcher::Insert(TableWithColumnIndexT& table_) {
        {
            SQLite::Transaction transaction(*_database);
            _database->exec(DeleteResultSet_);
            _database->exec(Createdb_);
            transaction.commit();
        }
        SQLite::Transaction transaction(*_database);
        SQLite::Statement   insertResultSet(*_database, InsertResultSetRow_);
        const std::string   exchange = ToString(Exchange_NSE_FUTURE);

        std::ranges::for_each(table_, [&insertResultSet, exchange](const RowWithColumnIndexT& row_) {
            insertResultSet.bind(1, row_[TOKEN]);
            insertResultSet.bind(2, row_[STRIKE]);
            insertResultSet.bind(3, row_[INSTRUMENT_TYPE]);
            insertResultSet.bind(4, row_[SYMBOL]);
            insertResultSet.bind(5, row_[EXPIRY]);
            insertResultSet.bind(6, row_[DESCRIPTION]);
            insertResultSet.bind(7, row_[OPTION_TYPE]);
            insertResultSet.bind(8, row_[DESCRIPTION]);
            insertResultSet.bind(9, exchange);
            insertResultSet.bind(10, "F&O");
            insertResultSet.bind(11, row_[FUTURE_TOKEN]);
            insertResultSet.bind(12, row_[LOT_SIZE]);
            insertResultSet.bind(13, row_[TICK_SIZE]);
            insertResultSet.bind(14, 100);
            insertResultSet.bind(15, row_[LOW_DPR]);
            insertResultSet.bind(16, row_[HIGH_DPR]);
            insertResultSet.bind(17, row_[LOW_DPR]);

            insertResultSet.exec();
            insertResultSet.reset();
        });
        transaction.commit();
    }

    auto ContractFetcher::GetResultWithColumnName(const std::string& query_) -> TableWithColumnNameT {
        SQLite::Statement    query(*_database, query_);
        TableWithColumnNameT table;
        while (query.executeStep()) {
            RowWithColumnNameT row;
            for (int i = 0; i < query.getColumnCount(); i++) {
                row.emplace(query.getColumnName(i), query.getColumn(i).getString());
            }
            table.push_back(row);
        }
        return table;
    }

    auto ContractFetcher::GetResultWithColumnIndex(const std::string& query_) -> TableWithColumnIndexT {
        SQLite::Statement     query(*_database, query_);
        TableWithColumnIndexT table;
        while (query.executeStep()) {
            RowWithColumnIndexT row;
            for (int i = 0; i < query.getColumnCount(); i++) {
                row.emplace_back(query.getColumn(i).getString());
            }
            table.push_back(row);
        }
        return table;
    }

    void ContractFetcher::ExecuteQuery(const std::string& query_) {
        try {
            SQLite::Statement query(*_database, query_);
            query.executeStep();
        } catch (std::exception& ex_) {
            LOG(ERROR, "{} {}", __FUNCTION__, ex_.what())
        }
    }
}  // namespace Lancelot
