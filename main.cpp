/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"

#include "smartmap.hpp"
#include "widgets/artnet_widget.hpp"
#include "widgets/file.hpp"
#include "widgets/stack.hpp"
// #include "boilerplate.hpp"
// #include "debug.hpp"
// Debug debug;

int main() { 

    // Boilerplate();

    auto &engine = Engine::getInstance();

    // SmartMap sm;

    // sm.import("config.json");

    // ArtnetWidget aw(sm.artnet);

    ShaderProgram shader({"basic.frag", "basic.vert"});

    ShaderProgram shadertex({"texture.frag", "basic.vert"});

    ShaderProgram shadersm({"smartmap.frag", "smartmap.vert"});

    FrameBuffer fb(engine.window.width,engine.window.height, "Layer1");
    
    engine.stack.list.push_back(new Stack::DrawCall{&engine.quad, &shadertex, nullptr, &fb, "Test to layer"});
    
    engine.stack.list.push_back(new Stack::DrawCall{&engine.quad, &shader, fb.texture, nullptr, "Test to window"});

    FileWidget fw;

    StackWidget sw(&engine.stack);

    Engine::Run();
 
}




    

