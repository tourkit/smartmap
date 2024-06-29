#ifndef __ENGINE_GL_WINDOW_HPP__
#define __ENGINE_GL_WINDOW_HPP__

#include <cstring>
#include <iostream>
#include <vector>
#include <functional>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


#include "output.hpp"
// #include "fps.hpp"

// #include "drawcall.hpp"
struct ShaderProgram; struct VBO; struct Texture;

struct Window : Output {

    struct Display { uint32_t width, height, rate, x, y; };

    std::vector<Display> displays;

    GLFWwindow *id;

    // FPS fps;

    bool fullscreen;

    bool stopped = false;

    uint32_t mouse_x, mouse_y, max_fps = 60;

    Window(uint32_t width, uint32_t height, uint32_t offset_x = 0, uint32_t offset_y = 0, bool fullscreen = false);

    void size(uint32_t width, uint32_t height) override;
    void pos(uint32_t offset_x, uint32_t offset_y) override;


    ~Window();

    void render(std::function<void()> callback = [](){});

    void draw() override;

    void keypress();

    std::function<void()> clickCallBack = []() { /*  PLOGD << "click"; */ };

    std::unordered_map<int, std::function<void(int)>> keypress_cbs;

};




#endif
