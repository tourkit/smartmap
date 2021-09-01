#ifndef __ENGINE_GL_WINDOW_HPP__
#define __ENGINE_GL_WINDOW_HPP__

#include "globals.hpp"

namespace GL {


struct Window {

    GLFWwindow *window;

    bool fullscreen;

    bool stopped = false;

    uint16_t width, height, fps, mouse_x, mouse_y, offset_x, offset_y;

    uint16_t MAX_FPS = 281;

    float iTime = 0;

    std::chrono::steady_clock::time_point start_c = std::chrono::steady_clock::now();

    Window(bool fullscreen, uint16_t width, uint16_t height, uint16_t offset_x = 0, uint16_t offset_y = 0);

    ~Window();

    void createView(int8_t windows_border = 1, int8_t window_on_top = 0, int8_t cursor_visibility = 1, int8_t uid_callbacks = 0);

    void render(std::function<void()> callback = [](){});

    void readRGB(char *rgb, uint16_t width, uint16_t height, uint16_t x = 0, uint16_t y = 0, uint8_t mode = 3);

    void initUidCallbacks();
    
    std::function<void()> clickCallBack = []() { /* std::cout << "click" <<std::endl; */ };

private:
    // SAD FACE EMOJI FPS  SUKI LAME

    double maxPeriod = 1.0/(float)MAX_FPS;
    double lastTime = 0.0;

    void getFPS();

};

}

#endif
