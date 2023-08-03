#ifndef __ENGINE_GL_WINDOW_HPP__
#define __ENGINE_GL_WINDOW_HPP__

#include <cstring>
#include <iostream>
#include <vector>
#include <functional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


struct FPS {

    std::string name;

    static inline std::vector<FPS*> pool;

    FPS(std::string name = "FPS");

    float current_time = 0, last_time = 0, fps = 1;

    float get();
    float run(float max = 0);

    operator float() { return fps; }

};

struct Window {

    struct Display { uint16_t width, height, rate; };
    std::vector<Display> displays;

    GLFWwindow *window;

    FPS fps; 
    
    bool fullscreen;

    bool stopped = false;

    uint16_t width, height, mouse_x, mouse_y, offset_x, offset_y, max_fps = 60;

    Window(bool fullscreen, uint16_t width, uint16_t height, uint16_t offset_x = 0, uint16_t offset_y = 0);
    
    void setSize(uint16_t width = 0, uint16_t height = 0);
    void setPos(uint16_t offset_x = 0, uint16_t offset_y = 0);
    void updateSize();
    void updatePos();

    ~Window();

    void render(std::function<void()> callback = [](){});

    void initUidCallbacks();
    
    std::function<void()> clickCallBack = []() { /* std::cout << "click" <<std::endl; */ };

};




#endif
