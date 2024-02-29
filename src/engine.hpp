#pragma once

#include "window.hpp"
#include "log.hpp"

struct Stack {};
struct Debug {};

struct GUI;
struct Node;
struct UBO;
struct DrawCall;
struct Atlas;

struct Engine {

    Log log;

    Window window;
    
    GUI* gui;
    
    UBO *dynamic_ubo;
    UBO *static_ubo;
    
    Atlas *atlas;

    Node* tree;
    Node* stack;
    Node* models;
    Node* effectors;
    Node* remaps;

    Node* selected = tree;

    Node *dc = nullptr;
    
    int sequid = 1000;

    std::string project_name;

    void init();
    void open(const char* filepath);

    static Engine& getInstance() { static Engine instance;  return instance; }

    void run();

private:

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    ~Engine();

};

static inline Engine &engine = Engine::getInstance();
