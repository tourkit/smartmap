#pragma once

#include "window.hpp"
#include "json.hpp"
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

struct Engine {

    // bool logger = Log::getInstance().cout(false);

    Node* tree;
    Node* stack;
    Node* models;
    Node* effectors;
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

    JSON json;

    void init();

    void reset();
    void open(const char* filepath);
    void save(const char* filepath);
    void save();

    static Engine& getInstance() { static Engine instance;  return instance; }

    void run();

    std::string project_filepath;

    void* debug_payload = nullptr;

    std::vector<std::string> available_ips;

    VBO *vbo;
    ShaderProgram *shader;

    static void Draw2D(Texture* texture);

private:

    std::string project_name;

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    ~Engine();

};

static inline Engine &engine = Engine::getInstance();
