#ifndef __ENGINE_GL_WINDOW_HPP__
#define __ENGINE_GL_WINDOW_HPP__

#include <cstring>
#include <vector>
#include <set>
#include <functional>
#include <map>
#include <memory>

#include "vendors/gl3w/include/GL/gl3w.h"
#include <GLFW/glfw3.h>


#include "output.hpp"
#include "fps.hpp"

// #include "drawcall.hpp"
struct ShaderProgram; struct VBO; struct Texture;

struct Window : Output {

    struct Display { uint32_t width, height, rate, x, y; };

    static inline std::vector<Display> displays;

    GLFWwindow *id;

    FPS fps;

    bool fullscreen;

    bool stopped = false;

    uint32_t mouse_x, mouse_y, max_fps = 60;

    Window(uint32_t width, uint32_t height, uint32_t offset_x = 0, uint32_t offset_y = 0, bool fullscreen = false);

    void size(uint32_t width, uint32_t height) override;
    void pos(int offset_x, int offset_y) override;


    ~Window();

    void render(std::function<void()> callback = [](){});

    void draw() override;

    void fit();

    void keypress();

    std::function<void()> clickCallBack = []() { /*  PLOGD << "click"; */ };
  
    std::set<int> keys_down;

    std::map<std::set<int>, std::function<void()>> keypress_cbs;
    
    static inline ShaderProgram *shader = nullptr;

    std::vector<std::pair<void*,std::function<void(void*)>>> end_of_render_cbs;
    std::function<void(double,double)> mousemove_cb = nullptr;
    std::function<void(int)> mousedown_cb = nullptr;

    void visibility(bool value);
    bool visibility();

private:

    bool visibility_v = false;

};




#endif
