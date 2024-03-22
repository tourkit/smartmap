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
#include "model.hpp"

#include "callbacks.hpp"

Engine::Engine(uint16_t width, uint16_t height) : window(1920,1080,2560,0) {

    window.max_fps = 59;

    gui = new GUI(window.id);

    dynamic_ubo = new UBO("dynamic_ubo");
    dynamic_ubo->striding(true);

    static_ubo = new UBO("static_ubo");
    static_ubo->striding(true);

    window.keypress_cbs[GLFW_KEY_ESCAPE] = [](int key) { exit(0); };

    window.keypress_cbs[GLFW_KEY_S] = [](int key) { engine.save("project2.json"); };
    
    window.keypress_cbs[GLFW_KEY_I] = [](int key) { engine.gui->draw_gui = !engine.gui->draw_gui; };

    
}

Engine::~Engine() { PLOGI << "Engine destroyed"; }

void Engine::init() {

    Callbacks::init(); 
    
    Editors::init();

    tree = new Node("tree");


    debug = tree->addOwnr<Debug>()->close()->node();
    debug->addPtr<UBO>(static_ubo);
    debug->addPtr<UBO>(dynamic_ubo);
    debug->addPtr<Atlas>(atlas);   

    atlas = new Atlas(4096, 4096, "assets/medias/");

    auto comps = debug->addOwnr<Node>("Components")->close();
    // for (auto c : Component::pool) comps->addPtr<Component>(c); // tofix

    models = tree->addOwnr<Node>("Models")->node();
    // models = tree->addFolder<Model>("Models", "assets/models/")->node();

    effectors = tree->addOwnr<Node>("Effectors")->node();
    // effectors = tree->addFolder<Effector>("Effectors", "assets/effectors/")->node();

    remaps = tree->addOwnr<Node>("Remaps")->node();

    inputs = tree->addOwnr<Node>("Inputs")->node()->active(1);

    outputs = tree->addOwnr<Node>("Outputs")->node();

    stack = tree->addOwnr<Stack>()->node()->active(1); 

    debug->select(); // NEEEEEED TO BE ONE SELECTED NODE !

    PLOGI << "Engine initialized";

}

void Engine::run() {

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){
        
        // engine.dynamic_ubo->upload();

        engine.tree->run();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        engine.gui->draw(); 
        
    });

}

void Engine::open(const char* file) {

    json.load(File(file).data.data());


    // for (auto e :engine.gui->editors) delete e; // editor widget deletion is fucked

    if (!json.loaded) { 

        if (!project_filepath.length()) {

            project_filepath = file;
            
            engine.gui->editors.push_back(new EditorWidget()); 

            engine.gui->editors.push_back(new EditorWidget()); 
            engine.gui->editors.back()->selected = debug;
            engine.gui->editors.back()->locked = true;

            auto f = debug->addOwnr<File>(project_filepath);
            f->onchange([](Node* n) { engine.open(engine.project_filepath.c_str()); });
            f->select();
        
        }

        return; 
        
    }

    project_name = file;

    if (true) for (auto &m : json["models"]) if (m.name.IsString() && m.value.IsString()) {

        auto n = models->addOwnr<File>();
        auto model = n->get();
        model->loadString(m.value.GetString());
        model->name = m.name.GetString();
        n->name = model->name;

    }

    if (true) for (auto &m : json["effectors"]) if (m.name.IsString() && m.value.IsString()) {

        auto n = effectors->addOwnr<File>();
        auto f = n->get();
        f->loadString(m.value.GetString());
        f->name = m.name.GetString();
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


            // tofix
            // // auto model = layer->add(model_n);
            // // Two two following lines are from NODE<Layer>::onadd<File> to replace above line wich is not working IDK why
            // layer->get()->vbo.import(model_n->is_a<File>());  // model_n might be fucked
            // auto model = layer->addPtr<Struct>(&layer->get()->vbo.models.back());

            // if (m.name.IsString()) model->name = m.name.GetString();
            
            // if (m.value.GetArray().Size() != 2) continue;
            // if (!m.value[1].IsArray()) continue;

            // for (auto &f : m.value[1].GetArray()) {

            //     if (!f.IsString()) continue;

            //     auto effector = effectors->child(f.GetString())->get<Effector>();

            //     if (effector) model->add(effector);
            //     else PLOGW << "no effector: " << f.GetString();

            // }

        }

        layer->update();
        
    }

    if (json.document.HasMember("editors") && json.document["editors"].IsArray()) for (auto &e : json.document["editors"].GetArray()) {

        if (!e.IsArray()) continue;

        auto x = e.GetArray();

        if (e.Size()<5) continue;

        engine.gui->editors.push_back(new EditorWidget());

        Node* n = nullptr;
        if (e[4].IsString()) n = tree->child(e[4].GetString());
        if (n) engine.gui->editors.back()->selected = n;

        if (e.Size() > 5 && e[5].IsBool() ) engine.gui->editors.back()->locked = e[5].GetBool();

    }


}

void Engine::save() { save(project_filepath.c_str()); }

void Engine::save(const char* file) {

    if (!json.document.HasMember("editors")) json.document.AddMember("editors", rapidjson::Value(rapidjson::kArrayType), json.document.GetAllocator());

    json.document["editors"].Clear();

    for (auto e : gui->editors) {
        
        auto v = rapidjson::Value(rapidjson::kArrayType);

        v.PushBack(0, json.document.GetAllocator());
        v.PushBack(0, json.document.GetAllocator());
        v.PushBack(0, json.document.GetAllocator());
        v.PushBack(0, json.document.GetAllocator());
        
        if (e->selected) v.PushBack(rapidjson::Value(e->selected->namesdf().c_str(), json.document.GetAllocator()), json.document.GetAllocator());
        if (e->locked) v.PushBack(rapidjson::Value(true), json.document.GetAllocator());

        auto &x = json.document["editors"].PushBack(v, json.document.GetAllocator());
        
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 2); // Set indent to 2 spaces
    json.document.Accept(writer);

    // inline from depth 
    std::string result = std::regex_replace(buffer.GetString(), std::regex(R"(\s{5}(([\]\}])|\s{2,}))"), " $2");
    // result = std::regex_replace(result, std::regex(R"(\n)"), " \n\n");

    File::write(file,result);

}
