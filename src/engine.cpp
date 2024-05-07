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

Engine::Engine(uint16_t width, uint16_t height) : window(1920,1080,2560,0), dynamic_ubo("dynamic_ubo"), static_ubo("static_ubo") {

    window.max_fps = 59;

    gui = new GUI(window.id);

    window.keypress_cbs[GLFW_KEY_ESCAPE] = [](int key) { exit(0); };

    window.keypress_cbs[GLFW_KEY_S] = [](int key) { engine.save("project2.json"); };

    window.keypress_cbs[GLFW_KEY_I] = [](int key) { engine.gui->draw_gui = !engine.gui->draw_gui; };

    tree = new Node("tree");

}

Engine::~Engine() { PLOGI << "Engine destroyed"; }
void Engine::init() {

    Callbacks::init();

    Editors::init();

    debug = tree->addOwnr<Debug>()->node();//->close();
    debug->addPtr<UBO>(&static_ubo)->onchange([](Node* n) { n->is_a<UBO>()->upload(); });
    debug->addPtr<UBO>(&dynamic_ubo);

    atlas = new Atlas(4096, 4096, "assets/medias/");
    debug->addPtr<Atlas>(atlas);

    // auto comps = debug->addOwnr<Node>("Components")->close();
    // for (auto c : Component::pool) comps->addPtr<Component>(c); // tofix

    models = tree->addOwnr<Node>("Models")->node();
    // models = tree->addFolder<File>("Models", "assets/models/")->node();

    effectors = tree->addOwnr<Node>("Effectors")->node();
    // effectors = tree->addFolder<File>("Effectors", "assets/effectors/")->node();

    remaps = tree->addOwnr<Node>("Remaps")->node();

    inputs = tree->addOwnr<Node>("Inputs")->node()->active(1);

    outputs = tree->addOwnr<Node>("Outputs")->node();

    stack = tree->addOwnr<Stack>()->node()->active(1);

    debug->select(); // NEEEEEED TO BE ONE SELECTED NODE !

    PLOGI << "Engine initialized";

}

void Engine::run() {

    if (!engine.gui->editors.size()) engine.gui->editors.push_back(new EditorWidget());

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){

        engine.dynamic_ubo.upload();

        engine.atlas->texture->bind();

        engine.tree->run();

        for (auto x : engine.stack->childrens) {

            auto l = x->is_a<Layer>();
            if (!l) continue;

            l->fb.texture->bind();
            //use quad(engine dracall maybe) to draw
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        engine.gui->draw();

    });

}

void Engine::open(const char* file) {

    json.load(File(file).data.data());

    project_filepath = file;

    if (!json.loaded) {

        engine.gui->editors.push_back(new EditorWidget());

        engine.gui->editors.push_back(new EditorWidget());
        engine.gui->editors.back()->selected = debug;
        engine.gui->editors.back()->locked = true;

        auto f = debug->addOwnr<File>(project_filepath);
        f->onchange([](Node* n) { engine.open(engine.project_filepath.c_str()); });
        f->select();


        return;

    }

    project_name = file;

    if (true) for (auto &m : json["models"]) if (m.name.IsString() && m.value.IsString()) {

        if (engine.models->child(m.name.GetString())) continue;
        auto n = models->addOwnr<File>();
        n->get()->loadString(m.value.GetString());
        n->get()->name_v = m.name.GetString();
        n->get()->extension = "obj";
        n->get()->path = engine.project_filepath;

        n->name(n->get()->name_v);

    }

    if (true) for (auto &m : json["effectors"]) if (m.name.IsString() && m.value.IsString()) {

        if (engine.effectors->child(m.name.GetString())) continue;
        auto n = effectors->addOwnr<File>();
        n->get()->loadString(m.value.GetString());
        n->get()->name_v = m.name.GetString();
        n->get()->path = engine.project_filepath;
        n->get()->extension = "glsl";
        n->name(m.name.GetString());

    }

    // kassded la famille. pour toi public :) #inlineclub
    for (auto &m : json["inputs"]) if (m.name.IsString() && m.value.IsArray()) if (!strcmp(m.name.GetString(),"artnet")) engine.inputs->addOwnr<Artnet>(((m.value.GetArray().Size() && m.value.GetArray()[0].IsString()) ? m.value.GetString() : nullptr ))->active(1);

    for (auto &l : json["layers"]) {

        if (!l.value.IsArray()) continue;

        auto info = l.value.GetArray();

        int width = window.width, height = window.height;

        int models_id = 0;

        if (info.Size() > 2) if (info[0].IsInt() && info[1].IsInt()) {

            width = info[0].GetInt(); height = info[1].GetInt();

            if (info[2].IsObject()) models_id = 2;

        }

        auto layer = stack->addOwnr<Layer>(width,height,l.name.IsString() ? l.name.GetString() : "");

        if (!info.Size()) continue;

        for (auto it = info[models_id].MemberBegin(); it != info[models_id].MemberEnd(); ++it) {

            if (it->value.IsArray() && it->value.GetArray().Size()) { // is object

                if (

                    !it->value.GetArray()[0].IsObject()
                    || it->value.GetArray()[0].GetObj().MemberCount() != 1
                    || it->value.GetArray()[0].GetObj().MemberBegin()->name.IsString()
                    || it->value.GetArray()[0].GetObj().MemberBegin()->value.IsString()

                ) continue;

                PLOGD << it->value.GetArray()[0].GetObj().MemberBegin()->name.GetString();


                if (it->value.GetArray().Size() < 2) continue;

            }
        }
        //     auto &m = *it;

            // if (!m.value.IsArray()) continue;

            // if (

            //     !m.value[0].IsObject()
            //     || m.value[0].GetObj().ObjectEmpty()
            //     || !m.value[0].GetObj().MemberBegin()->value.IsString()
            //     || !m.value[0].GetObj().MemberBegin()->name.IsString()

            //  ) continue;

            // auto model_f = engine.models->child(m.value[0].GetObj().MemberBegin()->value.GetString()); if (!model_f)  { PLOGE << "no model : " << m.value[0].GetObj().MemberBegin()->value.GetString(); continue; }

            // auto model = layer->add(model_f);

            // if (m.value[0].GetObj().MemberBegin()->name.IsString()) model->name(m.value[0].GetObj().MemberBegin()->name.GetString());

            // if (m.value.GetArray().Size() < 2) continue;
            // if (!m.value[1].IsArray()) continue;

            // if (m.value.Size() > 2 && m.value[2].IsInt()) model->is_a<Model>()->s.quantity( m.value[2].GetInt() );

            // for (auto &f : m.value[1].GetArray()) {

            //     if (!f.IsString()) continue;

            //     auto effector = effectors->child(f.GetString())->get<Effector>();

            //     if (effector) model->add(effector);
            //     else PLOGE << "no effector: " << f.GetString();

            // }

        // }

        // if (l.value.GetArray().Size() > 1 && l.value.GetArray()[1].IsArray()) for (auto &m : l.value.GetArray()[1].GetArray()) {

        // //     if (!m.IsString()) PLOGW << "WAAAAAAAAAAAAAAAAAAAA------";

        // //     auto *f = effectors->child( m.GetString() );

        // //     if (!f) PLOGW << "WAAAAAAAAAAAAAAAAAAAA------";

        // //     layer->add(f);
        // //     // PLOGD << "add " << m.GetString() << " to " << layer->name;

        // }




        // layer->update();

    }

    if (json.document.HasMember("editors") && json.document["editors"].IsArray()) for (auto &e : json.document["editors"].GetArray()) {

        if (!e.IsArray()) continue;

        auto x = e.GetArray();

        if (e.Size()<5) continue;

        engine.gui->editors.push_back(new EditorWidget());


            Node* n = nullptr;
            if (e[4].IsString()) n = tree->child(e[4].GetString());
            if (n) {

                engine.gui->editors.back()->selected = n;

                engine.gui->editors.back()->locked = true;

            }

            // if (e.Size() > 5 && e[5].IsBool() ) engine.gui->editors.back()->locked = e[5].GetBool();


    }

}

void Engine::save() { save(project_filepath.c_str()); }

void Engine::save(const char* file) {

    if (!json.document.HasMember("editors")) json.document.AddMember("editors", rapidjson::Value(rapidjson::kArrayType), json.document.GetAllocator());
    if (!json.document.HasMember("models")) json.document.AddMember("models", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());
    if (!json.document.HasMember("effectors")) json.document.AddMember("effectors", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());
    if (!json.document.HasMember("layers")) json.document.AddMember("layers", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());

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


    json.document["models"].RemoveAllMembers();
    for (auto m : models->childrens) { json.document["models"].AddMember(rapidjson::Value(m->name().c_str(), json.document.GetAllocator()), rapidjson::Value(&m->is_a<File>()->data[0], json.document.GetAllocator()), json.document.GetAllocator()); }

    json.document["effectors"].RemoveAllMembers();
    for (auto m : effectors->childrens) {

        json.document["effectors"].AddMember(rapidjson::Value(m->name().c_str(), json.document.GetAllocator()), rapidjson::Value(&m->is_a<File>()->data[0], json.document.GetAllocator()), json.document.GetAllocator());

    }

    json.document["layers"].RemoveAllMembers();
    for (auto m : stack->childrens) {

        json.document["layers"].AddMember(rapidjson::Value(m->name().c_str(), json.document.GetAllocator()), rapidjson::Value(rapidjson::kArrayType), json.document.GetAllocator());

        rapidjson::Value new_layer;
        new_layer.SetObject();

        auto layer = m->is_a<Layer>();

        for (auto model : layer->models) {

            // rapidjson::Value myQuad;
            // myQuad.SetArray();

            // myQuad.PushBack(rapidjson::Value().SetString(model.get()->file->name().c_str(), json.document.GetAllocator()), json.document.GetAllocator());
            // myQuad.PushBack(rapidjson::Value().SetArray(), json.document.GetAllocator());

            // for(auto e : model.get()->effectors) {

            //     myQuad[1].PushBack(rapidjson::Value().SetString(e.get()->ref->name().c_str(), json.document.GetAllocator()), json.document.GetAllocator());
            // }

            // myQuad.PushBack(2, json.document.GetAllocator());

        }

    }




    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 2); // Set indent to 2 spaces
    json.document.Accept(writer);

    // inline from depth
    std::string result = std::regex_replace(buffer.GetString(), std::regex(R"(\s{5}(([\]\}])|\s{2,}))"), " $2");
    // result = std::regex_replace(result, std::regex(R"(\n)"), " \n\n");

    // PLOGD << result;
    File::write(file,result);

    PLOGD << "SAVED to " << file;

}
