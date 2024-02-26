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
#include "ndi.hpp"
#include "texture.hpp"

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

        // engine.tree->each<Texture>([](Node *n, Texture* tex) { tex->bind(); });

        engine.tree->run();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // for (auto n : engine.stack->childrens) {

        //     auto layer = n->is_a<Layer>(); 

        //     if (!layer) continue;

        //     layer->fb.texture->bind();


        // }

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

    stack = tree->addOwnr<Stack>()->select()->node();
    stack->active = true; 

    // auto atlas = tree->addOwnr<Atlas>(4096, 4096, "assets/media/")->select();

    PLOGD << "Engine initialized";

    ///////////////////////////////////////////////////////////////////

    auto layer1 = stack->addOwnr<Layer>()->select();
    auto model = layer1->addPtr(models->childrens[0]); 
    model->addPtr(shaders->childrens[0]); 
    model->addPtr(shaders->childrens[2]); 
    // model->addPtr(shaders->childrens[1]); 
    
    // atlas->get()->link(&layer1->get()->shader);

    auto an = tree->addOwnr<Artnet>();
    an->active = true;

    gui->editors.back()->selected = layer1->node();
    // gui->editors.back()->locked = true;
    gui->editors.push_back(new EditorWidget());
    gui->editors.back()->selected = model;
    gui->editors.back()->locked = true;
    gui->editors.push_back(new EditorWidget());
    gui->editors.back()->selected = an->node();
    gui->editors.back()->locked = true;
    gui->editors.push_back(new EditorWidget());
    gui->editors.back()->selected = stack;
    gui->editors.back()->locked = true;

    auto fixture = new DMX::Fixture(model->is_a<Model>()->obj->s);
    // fixture->attributes[0].combining = 0;
    fixture->attributes[4].combining = 2;
    fixture->attributes[5].combining = 2;
    fixture->attributes[6].combining = 2;
    fixture->attributes[7].combining = 2;
    fixture->attributes[8].combining = 0;

    auto m = model->is_a<Model>();

    an->get()->universes[0] = new DMX(0);
    an->get()->universes[0]->remaps.push_back(DMX::Remap(&an->get()->universes[0]->data[0], m->obj->data(), fixture, 1));
    an->trigchange();

    auto ndi = tree->addOwnr<NDI::Sender>(engine.window.width,engine.window.height);
    
    
    // dc->is_a<Layer>()->shader.sendUniform("texture0", (int)tex->id);
    
    // tex->bind();


    // auto tex = new Texture("assets/media/boy.jpg");

    // ndi->onrun([](Node* n) { 

    //     auto sender = n->is_a<NDI::Sender>();
    //     sender->send()

    //  });

    // auto p = tree->addOwnr<Node>("prout");
    // delete p;



    // auto dc = tree->addOwnr<DrawCall>();;
    // this->dc = dc->node();
    // dc->name = "engine";

    // dc->addPtr(models->childrens[0]);

}