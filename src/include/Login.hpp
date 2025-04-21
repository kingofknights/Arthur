#pragma once

#include "include/Structure.hpp"
class Login {
  public:
    Login();

    void Paint();

    [[nodiscard]] auto IsLoggedIn() const noexcept -> bool;

    [[nodiscard]] auto GetUserDetails() const noexcept -> UserDetails;

  private:
    std::string _user;
    std::string _password;
    std::string _error;
    int         _attempt = 5;
    UserDetails _userDetails;
};
