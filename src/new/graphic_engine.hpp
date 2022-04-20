#ifndef SMARTMAP_ECS_HPP
#define SMARTMAP_ECS_HPP
#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

struct GraphicEngine {

    GLFWwindow *window;

    uint16_t width, height, fps, mouse_x, mouse_y, offset_x, offset_y;

    Window(bool fullscreen, uint16_t width, uint16_t height, uint16_t offset_x = 0, uint16_t offset_y = 0);

    ~GraphicEngine() { glfwTerminate(); }

    void createView(int8_t windows_border = 1, int8_t window_on_top = 0, int8_t cursor_visibility = 1, int8_t uid_callbacks = 0);

    void render(std::function<void()> callback = [](){});

    void initUidCallbacks();
    
    std::function<void()> clickCallBack = []() { /* std::cout << "click" <<std::endl; */ };

};

}

#endif
