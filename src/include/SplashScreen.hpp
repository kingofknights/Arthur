//
// Created by VIKLOD on 22-04-2023.
//
#pragma once
typedef unsigned int GLuint;

class SplashScreen {
  public:
    SplashScreen();
    void paint();

  protected:
    void DrawSplashScreen();

  private:
    bool   _loadDatabase = true;
    int    _width        = 0;
    int    _height       = 0;
    int    _count        = 0;
    GLuint _image        = 0;
};