#ifndef __ENGINE_GL_WINDOW_HPP__
#define __ENGINE_GL_WINDOW_HPP__

#include <cstring>
#include <iostream>
#include <vector>
#include <functional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


#include "fps.hpp"


struct Window {

    struct Display { uint16_t width, height, rate; };
    std::vector<Display> displays;

    GLFWwindow *id;

    FPS fps; 
    
    bool fullscreen;

    bool stopped = false;

    uint16_t width, height, mouse_x, mouse_y, offset_x, offset_y, max_fps = 60;

    Window(uint16_t width, uint16_t height, uint16_t offset_x = 0, uint16_t offset_y = 0, bool fullscreen = false);
    
    void setSize(uint16_t width = 0, uint16_t height = 0);
    void setPos(uint16_t offset_x = 0, uint16_t offset_y = 0);
    void updateSize();
    void updatePos();

    ~Window();

    void render(std::function<void()> callback = [](){});

    void keypress();
    
    std::function<void()> clickCallBack = []() { /*  PLOGD << "click"; */ };

};




#endif
