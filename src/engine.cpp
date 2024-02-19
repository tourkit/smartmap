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

#include "callbacks.hpp"

Engine::Engine(uint16_t width, uint16_t height) : window(1920,1080,2560,0) {

    window.max_fps = 59;

    gui = new GUI(window.id);

    dynamic_ubo = new UBO("dynamic_ubo");

    static_ubo = new UBO("static_ubo");

}

Engine::~Engine() { PLOGD << "Engine destroyed"; }
 
void Engine::run() {

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){
        
        engine.dynamic_ubo->upload();

        engine.tree->run();

        engine.gui->draw(); 
        
    });

};

template <typename T>
static Node* addFolder(std::string name, std::string path) {


    auto folder = engine.tree->addOwnr<Node>(name);
    folder->onadd<File>([](Node* _this, Node* node){ _this->addOwnr<T>(node->is_a<File>())->referings.push_back(node); return _this; });
    folder->addList(&folder->addOwnr<Directory>(path)->hide()->childrens);

    return folder->node();

}

void Engine::init() {

    Callbacks::init();
    
    Editors::init();

    tree = new Node("tree");

    auto models = addFolder<Model>("Model", "assets/model/");

    auto shaders = addFolder<Effector>("Shaders", "assets/shaders/");

    stack = tree->addOwnr<Stack>()->node();
    // stack->active = true;

    PLOGD << "Engine initialized";

    ///////////////////////////////////////////////////////////////////


    auto dc = stack->addOwnr<DrawCall>()->select();
    auto model = dc->addPtr(models->childrens[0]); 
    model->addPtr(shaders->childrens[2]); 
    model->addPtr(shaders->childrens[0]); 

    // auto an = controllers->add(new Node{"Art-Net"});3

    // atlas = (Atlas*)tree->add(new Atlas(4096, 4096, "assets/media/"));

}