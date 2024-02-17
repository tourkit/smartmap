#pragma once

#include "window.hpp"
#include "ubo.hpp"

#include "log.hpp"
#include "node.hpp"
#include "atlas.hpp"
#include "buffer.hpp"

#include "gui.hpp"

struct Stack {};

struct Engine {

    Log log;

    Window window;

    GUI* gui;

    Buffer buffer; 

    UBO *dynamic_ubo;

    UBO *static_ubo;

    Node* tree = new Node("tree");

    Node* stack = tree->addOwnr<Stack>()->node();

    Node* selected = tree;
    
    Atlas *atlas;
    
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