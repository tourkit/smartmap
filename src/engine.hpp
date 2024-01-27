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

    GUI gui;
    
    // UBO dynamic_ubo, static_ubo;

    Tree tree;
    
    Node* stack = tree.add(new Node{"Stack"});

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
