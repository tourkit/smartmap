#include "engine.hpp"
#include "directory.hpp"
#include "struct.hpp"
#include "shaderfx.hpp"
#include "drawcall.hpp"

#include "nodes.hpp"

Engine::Engine(uint16_t width, uint16_t height) 
    : window(1920,1080,2560,0), 
     gui(window.id) {
    window.max_fps = 59;

    dynamic_ubo = new UBO("dynamic_ubo");
    static_ubo = new UBO("static_ubo");

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

    stack->addPtr<UBO>(dynamic_ubo)->select();

    tree->addPtr<UBO>(static_ubo);

    auto models = tree->add<Directory>("assets/model/");
    auto shaders = tree->add<Directory>("assets/shaders/");

    // auto dc = stack->add<DrawCall>();

    // dynamic_ubo->subscribers.push_back(&dc->get()->shader);

    // auto model = dc->addPtr(models->childrens[0]); 

    // model->addPtr(shaders->childrens[0]); 


    
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