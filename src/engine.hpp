#pragma once

#include "window.hpp"
#include "ubo.hpp"
#include "gui.hpp"

#include "node.hpp"
#include "atlas.hpp"


#include "tree.hpp"
#include "editor.hpp"

struct Engine {

    Window window;

    // UBO dynamic_ubo, static_ubo;

    int sequid = 1000;

    Atlas *atlas;

    Tree tree;
    
    Node* stack = tree.add(new Node{"Stack"});
    
    GUI gui;

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
