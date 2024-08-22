#pragma once

#include "window.hpp"
#include "framebuffer.hpp"
#include "node.hpp"
#include "ubo.hpp"
#include "log.hpp"


struct Stack {  };

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

    Node* tree;
    Node* stack;
    Node* models;
    Node* effectors;
    Node* medias;
    Node* remaps;
    Node* inputs;
    Node* outputs;
    Node* timelines;
    Node* debug;

    Node* selected;

    Window window;

    GUI* gui;

    UBO dynamic_ubo, static_ubo;

    Atlas *atlas;

    void init();

    void reset();
    void open(const char* filepath);
    void save(const char* filepath);
    void save();

    static Engine& getInstance() { static Engine instance;  return instance; }

    std::vector<FrameBuffer> render_passes;

    void run();

    std::string project_filepath;

    static inline auto glsl_data = Member("ENGINE").add<int>("frame").add<int>("fps").add<int>("alt");

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
