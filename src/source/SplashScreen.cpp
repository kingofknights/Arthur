//
// Created by VIKLOD on 22-04-2023.
//

#include "../include/SplashScreen.hpp"

#include "../include/Structure.hpp"
//
/*
 * NOTE : bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
 * This function is copied from
 * https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
 * and implementation is in main.cpp
 */

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);

SplashScreen::SplashScreen() {
    bool ret = LoadTextureFromFile("SplashScreen.png", &_image, &_width, &_height);
    IM_ASSERT(ret);
}

void SplashScreen::paint() { DrawSplashScreen(); }

void SplashScreen::DrawSplashScreen() {
    if (ImGui::Begin("Splash Screen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Image((void*)(intptr_t)_image, ImVec2(_width, _height));
    }
    ImGui::End();
}
