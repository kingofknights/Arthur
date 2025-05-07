#pragma once

#include "include/Structure.hpp"

#include <pqxx/pqxx>

using UserIdT = int;

class Postgres;
using PostgresPtrT = std::unique_ptr<Postgres>;

class Postgres final {

  public:
    using ConnectionParamT = struct {
        std::string _host;
        std::string _user;
        std::string _password;
        std::string _database;
    };

    explicit Postgres(const ConnectionParamT& param_);

    [[nodiscard]] static auto GetConnectionStr(const ConnectionParamT& param_) -> std::string;

    auto CheckValidUser(const std::string& loginId_, const std::string& password_) -> UserDetails;

    static PostgresPtrT PostgresPtr;

  private:
    pqxx::connection _connection;
};
