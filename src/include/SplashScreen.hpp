#pragma once

using GLuint = unsigned int;

class SplashScreen {
  public:
    SplashScreen();

    void Paint();

  protected:
    void DrawSplashScreen();

  private:
    bool   _loadDatabase = true;
    int    _width        = 0;
    int    _height       = 0;
    int    _count        = 0;
    GLuint _image        = 0;
};
