#pragma once

#include "pch.hpp"

#include "window.hpp"
#include "ubo.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "stack.hpp"
#include "gui.hpp"
#include "model.hpp"
#include "shaderFX.hpp"
#include "drawcall.hpp"

#include "node.hpp"
#include "atlas.hpp"

#include "widgets/buffer.hpp"
#include "widgets/textures.hpp"


#include "tree.hpp"
#include "editor.hpp"

struct Engine {

    Window window;

    UBO dynamic_ubo, static_ubo;

    Buffer::Object *framebuffers, *matrices;
    Buffer::Object::Entry specs;
    int sequid = 1000;

    ShaderProgram *basicshader;

    VBO *quad;

    Atlas *atlas;

    // Stack stack;

    Node* stack = tree.add(new Node{"Stack"});
    
    GUI gui;

    BufferWidget bw;
    TexturesWidget tw;

    std::vector<Model*> models;
    std::vector<ShaderFX*> shaders;

    std::array<char,512> blank{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    void init();

    static Engine& getInstance() { static Engine instance;  return instance; }

    void run();

    Tree tree;
    EditorWidget editorw;

private:

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    ~Engine();

};
