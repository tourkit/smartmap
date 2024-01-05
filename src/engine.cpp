#include "engine.hpp"


Engine::Engine(uint16_t width, uint16_t height) 

    : window(1920,1080), 
    // : window(width,height), 
    dynamic_ubo("dynamic_ubo"), static_ubo("static_ubo"), 
     gui(window.id) {
    window.max_fps = 59;



    PLOGD << "Engine created";

    // window.max_fps

}

Engine::~Engine() { PLOGD << "Engine destrioyed"; }

void Engine::init() {

    framebuffers = static_ubo.buffer.add("Framebuffer", {"int", "int","int", "int"}, 100 );
    matrices = static_ubo.buffer.add("Matrice", {"Size", "Position", "Position", "Position"}, 100);
    // specs = dynamic_ubo.buffer.add("specs", {"int","int","int","int"},1)->create();
    
    for (auto file : Directory(REPO_DIR+"/assets/model/").list)  tree.add(new Model(file));
    for (auto file : Directory(REPO_DIR+"/assets/shaders/").list)  tree.add(new ShaderFX(file));

    float plain[8] = {1,1,0,0,.5,.5,0,0};
    matrices->push(&plain[0]);

    basicshader = new ShaderProgram({"basic.frag", "basic.vert"});

    quad = new VBO("quad.obj", 0, "quad");

    stack.list.push_back(new Stack::Action{[](){ 

        for (int i = 0; i< File::pool.size(); i++) { 

            File* file = File::pool[i];

            if (file->survey && file->last_modified) {

                auto new_modified = file->getTimeModified();

                if (file->last_modified != new_modified) file->callback(file);      

            }

        }

     }, "Files survey"});

     PLOGD << "Engine initialized";

}