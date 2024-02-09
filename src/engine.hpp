#pragma once

#include "window.hpp"
#include "ubo.hpp"
#include "gui.hpp"

#include "node.hpp"
#include "atlas.hpp"


#include "tree.hpp"
#include "editor.hpp"
#include "buffer.hpp"

struct Stack {};

struct Engine {

    Window window;

    GUI gui;

    Buffer buffer; 
    
    NODE<Tree>* tree = new NODE<Tree>();

    UBO *dynamic_ubo;

    UBO *static_ubo;

    NODE<Stack>* stack = tree->add<Stack>();
    
    Atlas *atlas;
    
    int sequid = 1000;

    void init();

    static Engine& getInstance() { static Engine instance;  return instance; }

    void run();

    EditorWidget editorw;

private:

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    ~Engine();

};

static inline Engine &engine = Engine::getInstance();