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
#include "json.hpp"

#include "callbacks.hpp"

Engine::Engine(uint16_t width, uint16_t height) : window(1920,1080,2560,0) {

    window.max_fps = 59;

    gui = new GUI(window.id);

    dynamic_ubo = new UBO("dynamic_ubo");

    static_ubo = new UBO("static_ubo");

}

Engine::~Engine() { PLOGI << "Engine destroyed"; }

void Engine::init() {

    Callbacks::init();
    
    Editors::init();

    atlas = new Atlas(4096, 4096, "assets/medias/");

    tree = new Node("tree");

    auto debug = tree->addOwnr<Debug>()->close();
    debug->addPtr<UBO>(static_ubo);
    debug->addPtr<UBO>(dynamic_ubo);
    debug->addPtr<Atlas>(atlas);
    auto comps = debug->addOwnr<Node>("Components")->close();
    for (auto c : Component::pool) comps->addPtr<Component>(c);

    models = tree->addOwnr<Node>("Models")->node();

    // effectors = tree->addOwnr<Node>("Effectors")->node();
    effectors = tree->addFolder<Effector>("Effectors", "assets/shaders/")->node();

    remaps = tree->addOwnr<Node>("Remaps")->node();

    inputs = tree->addOwnr<Node>("Inputs")->node()->active(1);

    outputs = tree->addOwnr<Node>("Outputs")->node();

    stack = tree->addOwnr<Stack>()->node()->active(1); 

    debug->select(); // NEEEEEED TO BE ONE SELECTED NODE !

    PLOGI << "Engine initialized";

    ///////////////////////////////////////////////////////////////////

}

void Engine::open(const char* file) {

    JSON json(File(file).data.data());

    project_name = file;

    for (auto &m : json["models"]) if (m.name.IsString() && m.value.IsString()) {

        auto n = models->addOwnr<File>();
        auto model = n->get();
        model->loadString(m.value.GetString());
        model->name = m.name.GetString();
        model->path = engine.project_name;
        models->addOwnr<Model>(model);
        n->hide();


    }

    if (false) for (auto &m : json["effectors"]) if (m.name.IsString() && m.value.IsString()) {

        auto n = effectors->addOwnr<File>();
        auto f = n->get();
        f->loadString(m.value.GetString());
        f->name = m.name.GetString();
        f->path = engine.project_name;
        effectors->addOwnr<Effector>(f);
        n->hide();

    }

    // kassded la famille. pour toi public :) #inlineclub
    for (auto &m : json["inputs"]) if (m.name.IsString() && m.value.IsArray()) if (!strcmp(m.name.GetString(),"artnet")) engine.inputs->addOwnr<Artnet>(((m.value.GetArray().Size() && m.value.GetArray()[0].IsString()) ? m.value.GetString() : nullptr ))->active(1);

    if (true) for (auto &l : json["layers"]) {
        
        auto layer = stack->addOwnr<Layer>();

        if (l.name.IsString()) layer->name = l.name.GetString();

        if (l.value.IsObject()) for (auto &m : l.value.GetObj()) {

            if (!m.value.IsArray()) continue;

            if (!m.value[0].IsString()) continue;

            auto model_n = models->child(m.value[0].GetString()); if (!model_n)  { PLOGW << "no model : " << m.value[0].GetString(); continue; }

            auto model = layer->addPtr(model_n);

            if (m.name.IsString()) model->name = m.name.GetString();
            
            if (m.value.GetArray().Size() != 2) continue;
            if (!m.value[1].IsArray()) continue;

            for (auto &f : m.value[1].GetArray()) {

                if (!f.IsString()) continue;

                auto effector = effectors->child(f.GetString())->get<Effector>();

                if (effector) model->addPtr(effector);
                else PLOGW << "no effector: " << f.GetString();

            }

        }
        
    }

}

void Engine::run() {

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){
        
        engine.dynamic_ubo->upload();

        engine.tree->run();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        engine.gui->draw(); 
        
    });

};
