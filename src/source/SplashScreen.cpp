//
// Created by VIKLOD on 22-04-2023.
//

#include "../include/SplashScreen.hpp"

#include "../include/Structure.hpp"

#include <GL/gl.h>
//
/*
 * NOTE : bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
 * This function is copied from
 * https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
 * and implementation is in main.cpp
 */
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {
    auto LoadTextureFromMemory(const void* data_, size_t data_size_, GLuint* out_texture_, int* out_width_, int* out_height_) -> bool {
        int            imageWidth  = 0;
        int            imageHeight = 0;
        unsigned char* imageData   = stbi_load_from_memory((const unsigned char*)data_, (int)data_size_, &imageWidth, &imageHeight, nullptr, 4);
        if (imageData == nullptr) {
            return false;
        }

        // Create a OpenGL texture identifier
        GLuint imageTexture;
        glGenTextures(1, &imageTexture);
        glBindTexture(GL_TEXTURE_2D, imageTexture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload pixels into texture
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
        stbi_image_free(imageData);

        *out_texture_ = imageTexture;
        *out_width_   = imageWidth;
        *out_height_  = imageHeight;

        return true;
    }

    auto LoadTextureFromFile(const char* file_, GLuint* out_, int* width_, int* height_) -> bool {
        FILE* file = fopen(file_, "rb");
        if (file == nullptr) {
            return false;
        }
        fseek(file, 0, SEEK_END);
        auto fileSize = static_cast<int>(ftell(file));
        if (fileSize == -1) {
            return false;
        }

        fseek(file, 0, SEEK_SET);
        void* fileData = IM_ALLOC(static_cast<size_t>(fileSize));
        fread(fileData, 1, static_cast<size_t>(fileSize), file);
        fclose(file);
        bool ret = LoadTextureFromMemory(fileData, static_cast<size_t>(fileSize), out_, width_, height_);
        IM_FREE(fileData);
        return ret;
    }
}  // namespace

SplashScreen::SplashScreen() {
    LoadTextureFromFile("SplashScreen.png", &_image, &_width, &_height);
}

void SplashScreen::paint() { DrawSplashScreen(); }

void SplashScreen::DrawSplashScreen() {
    if (ImGui::Begin("Splash Screen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Image(_image, ImVec2(_width, _height));
    }
    ImGui::End();
}
