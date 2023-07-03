#ifndef __ENGINE_GL_WINDOW_HPP__
#define __ENGINE_GL_WINDOW_HPP__

#include "globals.hpp"

namespace GL {

struct FPS {

    static inline std::vector<FPS*> pool;

    FPS();

    float current_time = 0, last_time = 0, fps = 1;

    float run(float max);

    operator float() { return fps; }

};

struct Window {

    GLFWwindow *window;

    FPS fps;
    
    bool fullscreen;

    bool stopped = false;

    uint16_t width, height, mouse_x, mouse_y, offset_x, offset_y;

    Window(bool fullscreen, uint16_t width, uint16_t height, uint16_t offset_x = 0, uint16_t offset_y = 0);
    
    void setSize(uint16_t width = 0, uint16_t height = 0);
    void setPos(uint16_t offset_x = 0, uint16_t offset_y = 0);
    void updateSize();
    void updatePos();

    ~Window();

    void createView(int8_t windows_border = 1, int8_t window_on_top = 0, int8_t cursor_visibility = 1, int8_t uid_callbacks = 0);

    void render(std::function<void()> callback = [](){});

    void initUidCallbacks();
    
    std::function<void()> clickCallBack = []() { /* std::cout << "click" <<std::endl; */ };

};



}

#endif
