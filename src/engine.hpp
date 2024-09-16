#pragma once

#include "window.hpp"
#include "framebuffer.hpp"
#include "node.hpp"
#include "globals.hpp"
#include "ubo.hpp"
#include "log.hpp"
#include <memory>
#include <vector>


struct Debug {};

struct GUI;
struct Node;
struct DrawCall;
struct Atlas;
struct ShaderProgram;
struct VBO;
struct Effector;

struct Engine {

    // bool logger = Log::getInstance().cout(false);

    Node* tree = nullptr;
    Node* stack = nullptr;
    Node* models = nullptr;
    Node* effectors = nullptr;
    Node* medias = nullptr;
    Node* structs = nullptr;
    Node* remaps = nullptr;
    Node* inputs = nullptr;
    Node* outputs = nullptr;
    Node* timelines = nullptr;
    Node* debug = nullptr;

    Window window;

    GUI* gui_v; 

    UBO* dynamic_ubo;
    UBO* static_ubo;

    void init();

    void reset();
    void open(const char* filepath);
    void save(const char* filepath);
    void save();

    std::vector<std::function<void()>> end_of_render_cbs;

    void gui(bool active);

    static Engine& getInstance() { static Engine instance;  return instance; }

    std::vector<FrameBuffer> render_passes;

    void run(std::function<void()> cb = [](){});

    std::string project_filepath;

    Member glsl_data;

    GLint gl_max_texture_size,gl_max_texture_image_units, gl_major_version, gl_minor_version;

private:

    std::string project_name;

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    ~Engine();

    Log &log = Log::getInstance();

};


static inline Engine &engine = Engine::getInstance();
