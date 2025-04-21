//
// Created by VIKLOD on 22-04-2023.
//

#include "../include/SplashScreen.hpp"

#include "Lancelot/Logger.hpp"

#include <imgui.h>
//
/*
 * NOTE : bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
 * This function is copied from
 * https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
 * and implementation is in main.cpp
 */

auto LoadTextureFromFile(const char* file_, GLuint* out_, int* width_, int* height_) -> bool;

SplashScreen::SplashScreen() {
    bool loaded = LoadTextureFromFile("SplashScreen.png", &_image, &_width, &_height);
    LOG(INFO, "SplashScreen: loaded {}", loaded);
}

void SplashScreen::Paint() { DrawSplashScreen(); }

void SplashScreen::DrawSplashScreen() {
    if (ImGui::Begin("Splash Screen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Image(_image, ImVec2(_width, _height));
    }
    ImGui::End();
}
