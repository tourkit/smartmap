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
    engine.init();

    SmartMap sm;

    sm.import("config.json");

    std::cout <<  "\nSHADERS HEADER:" << std::endl;
    for (auto ubo:UBO::pool) { 

        std::string str = "layout (binding = ";
        str += std::to_string(ubo->binding);
        str += ", std140) uniform ";
        str += ubo->name;
        str += " { ";
        
        int i = 0; for (auto obj:ubo->buffer.objects) { 

            str += obj.name;
            str += " ";
            for(int i = 0; i < obj.name.length(); i++) str += std::tolower(obj.name[i]); 
            str += "[";
            str += std::to_string(obj.reserved);
            str += "]; ";

         }

        str += " };";
        std::cout <<  str << std::endl;

    }

    // ArtnetWidget aw(sm.artnet);

    // ShaderProgram shader({"basic.frag", "basic.vert"});

    // ShaderProgram shadertex({"texture.frag", "basic.vert"});

    // ShaderProgram shadersm({"smartmap.frag", "smartmap.vert"});

    // ShaderProgram shaderfeed({"feedback.frag", "smartmap.vert"});

    // VBO feedbackquad;

    // FrameBuffer feedback(engine.window.width,engine.window.height, "Feedback");

    // FrameBuffer layer1(engine.window.width,engine.window.height, "Layer1");

    // engine.stack.list.push_back(new Stack::DrawCall{engine.quad, &shader, nullptr, &layer1, "Quad to layer"});

    // // engine.stack.list.push_back(new Stack::DrawCall{engine.quad, &shadertex, feedback.texture, &layer1, "Feedback to layer"});

    // engine.stack.list.push_back(new Stack::DrawCall{engine.quad, &shadertex, layer1.texture, &feedback, "layer to Feedback"});

    // engine.stack.list.push_back(new Stack::DrawCall{engine.quad, &shadertex, layer1.texture, nullptr, "layer to window"});

    // engine.stack.list.push_back(new Stack::DrawCall{engine.quad, &shader, nullptr, nullptr, "Quad to layer"});

    FileWidget fw;

    // StackWidget sw(&engine.stack);

    Engine::Run();
 
}


