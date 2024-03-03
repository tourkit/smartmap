#pragma once

#include "window.hpp"
#include "json.hpp"

struct Stack {};
struct Debug {};

struct GUI;
struct Node;
struct UBO;
struct DrawCall;
struct Atlas;

struct Engine {

    Node* tree;
    Node* stack;
    Node* models;
    Node* effectors;
    Node* remaps;
    Node* inputs;
    Node* outputs;

    Node* selected;

    Window window;
    
    GUI* gui;
    
    UBO *dynamic_ubo;

    UBO *static_ubo;
    
    Atlas *atlas;
    
    JSON json;

    void init();

    void open(const char* filepath);
    void save(const char* filepath);

    static Engine& getInstance() { static Engine instance;  return instance; }

    void run();

private:

    std::string project_name;

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    ~Engine();

};

static inline Engine &engine = Engine::getInstance();
