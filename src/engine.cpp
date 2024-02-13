#include "engine.hpp"
#include "directory.hpp"
#include "struct.hpp"
#include "shaderfx.hpp"
#include "drawcall.hpp"

#include "nodes.hpp"

Engine::Engine(uint16_t width, uint16_t height) 
    : window(1920,1080), 
    // : window(width,height), 
     gui(window.id) {
    window.max_fps = 59;

    PLOGD << "Engine created";

}

Engine::~Engine() { PLOGD << "Engine destroyed"; }

void Engine::run() {

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){
        
        auto &engine = Engine::getInstance();

        engine.stack->run();

        engine.gui.draw(); 

    });

};

void Engine::init() {

    Nodes::init();

    auto models = tree->add<Directory>("assets/model/");
    auto shaders = tree->add<Directory>("assets/shaders/");

    dynamic_ubo = tree->add<UBO>("dynamic_ubo")->get();
    static_ubo = tree->add<UBO>("static_ubo")->get(); 

    auto dc = stack->add<DrawCall>();
    auto model = dc->addPtr(models->childrens[0]); // maybe addPtr is not a good name
    model->addPtr(shaders->childrens[0]); 

    // Components::getInstance();
    // Node* comps = tree->add(new Node{"Components"});
    // for (auto c : Component::pool) comps->add(c);
    
    // Node* controllers = tree->add(new Node{"Controllers"});
    // auto an = controllers->add(new Node{"Art-Net"});
    // an->add(new Node{"1"});
    // an->add(new Node{"2"});
    // an->add(new Node{"3"});
    // an->add(new Node{"4"});

    // atlas = (Atlas*)tree->add(new Atlas(4096, 4096, "assets/media/"));
    
    // stack.list.push_back(new Stack::Action{[](){ 

    //     for (int i = 0; i< File::pool.size(); i++) { 

    //         File* file = File::pool[i];

    //         if (file->survey && file->last_modified) {

    //             auto new_modified = file->getTimeModified();

    //             if (file->last_modified != new_modified) file->callback(file);      

    //         }

    //     }

    //  }, "Files survey"});

     PLOGD << "Engine initialized";

}