#include "Postgres.hpp"

#include "Lancelot/Logger.hpp"
#include "include/Structure.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <pqxx/pqxx>
#include <string>

constexpr static char LoginQuery[] = R"(
  select 
  u."UserId" as user, 
  u."PersonId" as person, 
  u."Id" as id,
  p."Firstname" as first,
  p."Lastname" as last
  from public."Users" u 
  join public."Persons" p on p."Id" = u."PersonId"
  where u."Login"='{}' and u. "IsActive"='true' and u."Password" = MD5('{}')
)";

PostgresPtrT Postgres::PostgresPtr;

Postgres::ErrorHandler::ErrorHandler(pqxx::connection_base& connection_) : pqxx::errorhandler(connection_) {}

auto Postgres::ErrorHandler::operator()(char const message_[]) noexcept -> bool {
    LOG(ERROR, "Postgres: Error thrown {}", message_);
    return false;
}
Postgres::Postgres(const ConnectionParamT& param_)
    : _connection(GetConnectionStr(param_)), _error(_connection) {
    LOG(INFO, "Postgres connection string : {}", GetConnectionStr(param_));
    LOG(INFO, "Postgres connection status : {}", _connection.is_open());
}

auto Postgres::GetConnectionStr(const ConnectionParamT& param_) -> std::string {
    return FORMAT("dbname={} user={} password={} hostaddr={}", param_._database, param_._user, param_._password, param_._host);
}

auto Postgres::CheckValidUser(const std::string& loginId_, const std::string& password_) -> UserDetails {
    UserDetails details;

    pqxx::nontransaction work(_connection);
    const std::string    query(FORMAT(LoginQuery, loginId_, password_));
    const pqxx::result   result = work.exec(query);
    for (const auto& row : result) {
        details._userId    = row["user"].as<UserIdT>();
        details._valid     = true;
        details._personId  = row["person"].as<std::string>();
        details._userHash  = row["id"].as<std::string>();
        details._firstName = row["first"].as<std::string>();
        details._lastName  = row["last"].as<std::string>();
    }

    return details;
}
