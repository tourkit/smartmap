#include "engine.hpp"
#include "directory.hpp"
#include "struct.hpp"
#include "effector.hpp"
#include "drawcall.hpp"
#include "gui.hpp"
#include "editor.hpp"
#include "file.hpp"
#include "gui.hpp"
#include "ubo.hpp"
#include "node.hpp"
#include "atlas.hpp"
#include "artnet.hpp"
#include "dmx.hpp"

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

    auto folder = engine.tree->addOwnr<Node>(name)->close();
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
    stack->active = true;

    PLOGD << "Engine initialized";


    ///////////////////////////////////////////////////////////////////

    auto dc = stack->addOwnr<DrawCall>()->select();
    auto model = dc->addPtr(models->childrens[0]); 
    model->addPtr(shaders->childrens[2]); 
    model->addPtr(shaders->childrens[0]); 


    // DMX::Fixture fixture;
    
    
    // fixture.presets = { 


    //     {"COLOR", {
    //         {"RGBA", {
    //             {"Red",         1,  &Component::id("RGB")->members[0]}, 
    //             {"Green",       1,  &Component::id("RGB")->members[1]}, 
    //             {"Blue",        1,  &Component::id("RGB")->members[2]},                            
    //             {"Dimmer",        1,  &Component::id("RGB")->members[3]},                            
    //     }}}},

    //     {"Position", {
    //         {"Position", { 
    //             {"Pos_X",       2,  &Component::id("Position")->members[0]}, 
    //             {"Pos_Y",       2,  &Component::id("Position")->members[1]},                                      
    //     }}}},

    //     {"Focus", {
    //         {"Focus", {
    //             {"Focus_X",     2,  &Component::id("Size")->members[0]}, 
    //             {"Focus_Y",     2,  &Component::id("Size")->members[1]},                                  
    //     }}}},

    // };


    auto an = tree->addOwnr<Artnet>();

    // an->get()->universes[0].remaps.push_back({0, 1, (float*)model->is_a<Model>()->obj->data() });

    // auto &remap = an->get()->universes[0].remaps.back();

    // for (auto &c:model->is_a<Model>()->obj->s->comps) { 

    //     for (auto &m :c->members) { 

    //         uint8_t combining = 0;

    //         bool breaker = false;

    //         for (auto &p:fixture.presets) {
                 
    //             if (breaker) { break; }

    //             for (auto &f:p.features) { 

    //                 if (breaker) { break; }
    //                 for (auto &a:f.attributes) { 

    //                   if (&m == a.member) {

    //                       combining = a.combining; 
    //                       breaker = true;
    //                       break;

    //                     }
    //                 }
    //             }
    //         }

    //         remap.attributes.push_back({combining, m.range_from, m.range_to});  
        
    //     }
    // }



    gui->editors.back()->locked = true;
    gui->editors.push_back(new EditorWidget());
    gui->editors.back()->selected = model;
    gui->editors.back()->locked = true;
    gui->editors.push_back(new EditorWidget());
    gui->editors.back()->selected = an->node();
    gui->editors.back()->locked = true;

    // atlas = (Atlas*)tree->add(new Atlas(4096, 4096, "assets/media/"));

}