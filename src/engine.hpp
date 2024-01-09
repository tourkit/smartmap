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

#include "node.hpp"
#include "atlas.hpp"

#include "widgets/buffer.hpp"
#include "widgets/textures.hpp"


#include "tree.hpp"
#include "editor.hpp"

struct AtlasNode : Atlas, Node {
    
    AtlasNode() : Atlas("assets/media/", 4096, 4096) { 
        
        name = "atlas"; 

        // add()
        
    }

    void editor() {


    }

};

struct Engine {

    Window window;

    UBO dynamic_ubo, static_ubo;

    Buffer::Object *framebuffers, *matrices;
    Buffer::Object::Entry specs;
    int sequid = 1000;

    ShaderProgram *basicshader;

    VBO *quad;

    AtlasNode *atlas;

    Stack stack;
    
    GUI gui;

    BufferWidget bw;
    TexturesWidget tw;

    std::vector<Model*> models;
    std::vector<ShaderFX*> shaders;

    void init();

    static Engine& getInstance() { static Engine instance;  return instance; }

    static void Run() {

        auto &window = getInstance().window;

        while (!glfwWindowShouldClose(window.id)) window.render([](){
            

            auto &engine = Engine::getInstance();
            
            engine.dynamic_ubo.upload();

            engine.stack.run();

            engine.gui.draw();
            
            engine.specs.set<uint64_t>(0,engine.sequid--);
            if (!engine.sequid) engine.sequid = 1000;
      

        });

    };

    Tree tree;
    EditorWidget editorw;
    
private:

    Engine(uint16_t width = 400, uint16_t height = 300);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    ~Engine();

};
