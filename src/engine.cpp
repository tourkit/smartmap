#include "engine.hpp"

Engine::Engine(uint16_t width, uint16_t height) 

    : window(width,height), 
    dynamic_ubo("dynamic_ubo"), static_ubo("static_ubo"), 
    quad("quad.obj",width,height, "quad"),
    fb(0,width,height,"mainFB"), gui(window.id) {



    stack.list.push_back(new Stack::Action{[](){ 

        for (int i = 0; i< File::pool.size(); i++) { 

            File* file = File::pool[i];

            if (file->survey && file->last_modified) {

                auto new_modified = file->getTimeModified();

                if (file->last_modified != new_modified) file->callback(file);      

            }

        }

     }, "Files survey"});
     

}