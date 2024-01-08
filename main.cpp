/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "ndi.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"
#include "widgets/fps.hpp"

#include "shader_builder.hpp"
#include "model.hpp"
#include "tree.hpp"

// shader sources
// clocks
// fix feedback blending

int main() { 

    Log log;
    
    auto &engine = Engine::getInstance();
    engine.init();

    SmartMap::Base sm;

    auto *x = engine.dynamic_ubo.buffer.add("infos", {"int","int","int","int"},4);
    engine.specs = x->create();x->create();x->create();x->create();

    ShaderBuilder sb;
    
    TreeWidget tree;

    struct DC : Node {

        DC() {

            onchild<Model>({[](){ std::cout << "Model child added" << std::endl; }});

        }
        
    };

    engine.tree.add(new DC());

    struct Foo : Node {};

    Node* node = new Foo();

    std::cout << typeid(*node).name() << std::endl;

    // sm.config.import("config.json");

    FileWidget fw;

    FPSWidget fpsw;

    Engine::Run();
 
}


