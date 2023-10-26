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
    ShaderProgram shadersm({"smartmap.frag", "smartmap.vert"});
    
    engine.stack.list.push_back(new Stack::DrawCall{&engine.quad, &shader, nullptr, nullptr, "Test"});

    engine.stack.list.push_back(new Stack::Action{[](){ 

        for (int i = 0; i< File::pool.size(); i++) { 

            File* file = File::pool[i];

            if (file->survey && file->last_modified) {

                auto new_modified = file->getTimeModified();

                if (file->last_modified != new_modified) file->callback(file);      

            }

        }

     }, "Files survey"});


    FileWidget fw;

    StackWidget sw(&engine.stack);

    Engine::Run();
 
}




    

