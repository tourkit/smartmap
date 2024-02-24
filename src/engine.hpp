#pragma once

#include "window.hpp"
#include "shader.hpp"
#include "drawcall.hpp"
#include "ubo.hpp"

#include "log.hpp"

struct Stack {};

struct GUI;
struct Node;
struct Atlas;

struct Engine {

    Log log;

    Window window;

    GUI* gui;

    UBO *dynamic_ubo;

    UBO *static_ubo;

    Node* tree;

    Node* stack;

    Node* selected = tree;
    
    Atlas *atlas;

    DrawCall *dc = nullptr;
    
    int sequid = 1000;

    void init();

    static Engine& getInstance() { static Engine instance;  return instance; }

    void run();

private:

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    ~Engine();

};

static inline Engine &engine = Engine::getInstance();
