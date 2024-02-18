#include "engine.hpp"
#include "directory.hpp"
#include "struct.hpp"
#include "effector.hpp"
#include "drawcall.hpp"
#include "gui.hpp"
#include "editors.hpp"
#include "file.hpp"
#include "gui.hpp"
#include "ubo.hpp"
#include "node.hpp"
#include "atlas.hpp"

#include "nodes.hpp"

Engine::Engine(uint16_t width, uint16_t height) : window(1920,1080,2560,0) {

    window.max_fps = 59;

    gui = new GUI(window.id);

    tree = new Node("tree");

    stack = tree->addOwnr<Stack>()->node();
    stack->active = true;

    dynamic_ubo = new UBO("dynamic_ubo");

    static_ubo = new UBO("static_ubo");

}

Engine::~Engine() { PLOGD << "Engine destroyed"; }
 
void Engine::run() {

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){
        
        auto &engine = Engine::getInstance();
        
        engine.dynamic_ubo->upload();

        engine.tree->run();

        engine.gui->draw(); 

    });

};

void Engine::init() {

    Nodes::init();
    
    Editors::init();

    PLOGD << "Engine initialized";

    ///////////////////////////////////////////////////////////////////

    auto models = tree->addOwnr<Directory>("assets/model/");
    auto shaders = tree->addOwnr<Directory>("assets/shaders/");
    auto dc = stack->addOwnr<DrawCall>();
    auto model = dc->addPtr(models->childrens[0]); 
    model->addPtr(shaders->childrens[1]); 
    model->addPtr(shaders->childrens[0]); 


    // auto an = controllers->add(new Node{"Art-Net"});

    // atlas = (Atlas*)tree->add(new Atlas(4096, 4096, "assets/media/"));

}