#pragma once

#include <string>

#include "imgui.h"

namespace Themes {

void AddIconFonts(const std::string& ttf_, float size_);
void ImGuiMaterialDarkStyle();
void ImGuiMaterialLightStyle();
void ImGuiOneDarkStyle();
void ImGuiOneLightStyle();
void ImGuiVibrantLightStyle();
void ImGuiArthurDarkStyle();
void ImGuiArthurLightStyle();

}// namespace Themes
