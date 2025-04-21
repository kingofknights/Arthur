#include "include/Login.hpp"

#include "IconsMaterialDesign.h"
#include "Lancelot/Logger.hpp"
#include "database/Postgres.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "include/Colors.hpp"
#include "include/Structure.hpp"
#include "include/Themes.hpp"
#include "misc/cpp/imgui_stdlib.h"
#include "nlohmann/json.hpp"

Login::Login() {
    std::fstream file("setting.json");
    if (file.is_open()) {
        nlohmann::json        json     = nlohmann::json::parse(file);
        const nlohmann::json& font     = json["font"];
        const nlohmann::json& postgres = json["postgres"];

        std::string fontFile = "Ruda-Bold.ttf";
        float       fontSize = 18.0F;
        font["file"].get_to(fontFile);
        font["size"].get_to(fontSize);

        Themes::AddIconFonts(fontFile, fontSize);

        Postgres::ConnectionParamT connectionParam{
            ._host     = postgres["host"].get<std::string>(),
            ._user     = postgres["user"].get<std::string>(),
            ._password = postgres["password"].get<std::string>(),
            ._database = postgres["name"].get<std::string>(),
        };
        Postgres::PostgresPtr = std::make_unique<Postgres>(connectionParam);
    } else {
        LOG(ERROR, "Config file not found : setting.json", false);
        exit(1);
    }
    _error = "Enter user details";
}
void Login::Paint() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
    if (ImGui::Begin("Login", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse)) {
        ImGui::InputText("User", &_user);
        ImGui::InputText("Password", &_password, ImGuiInputTextFlags_Password);
        ImGui::Separator();
        ImGui::Columns(2);
        ImGui::BeginDisabled(_attempt == 0 or (_user.empty() or _password.empty()));
        if (ImGui::Button(ICON_MD_LOGIN " Login", ImVec2(-FLT_MIN, 0))) {
            LOG(INFO, "Login Window [._user = {}, ._password = {}]", _user, _password);
            _userDetails = Postgres::PostgresPtr->CheckValidUser(_user, _password);
            if (not _userDetails._valid) {
                _attempt -= 1;
                _error = FORMAT("{} Invalid login details, attempt left {}", ICON_MD_WARNING, _attempt);
            }
        }
        ImGui::EndDisabled();

        ImGui::NextColumn();
        if (ImGui::Button(ICON_MD_EXIT_TO_APP " Exit", ImVec2(-FLT_MIN, 0))) {
            std::exit(0);
        }
        ImGui::Separator();
        ImGui::EndColumns();
        ImGui::AlignTextToFramePadding();
        if (_attempt == 0) {
            ImGui::TextColored(COLOR_RED, ICON_MD_WARNING " Too many login attempt! Contact Admin");
        } else {
            ImGui::Text("%s", _error.data());
        }
    }
    ImGui::End();
}
auto Login::IsLoggedIn() const noexcept -> bool {
    return _userDetails._valid;
}
auto Login::GetUserDetails() const noexcept -> UserDetails {
    return _userDetails;
}
