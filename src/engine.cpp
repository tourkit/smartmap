#include "engine.hpp"
#include "folder.hpp"
#include "struct.hpp"
#include "effector.hpp"
#include "layer.hpp"
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
#include "utils.hpp"

#include "callbacks.hpp"

extern "C" {

    iface_t* artnet_list_ifaces();
    iface_t* artnet_list_ifaces_next(iface_t* ift);
    void artnet_free_ifaces(iface_t* ift);

}

Engine::Engine(uint16_t width, uint16_t height) : window(1,1,0,0), dynamic_ubo("dynamic_ubo"), static_ubo("static_ubo"),layers_s("Layer") {

    glGetIntegerv(GL_MAJOR_VERSION, &gl_major_version);
    glGetIntegerv(GL_MINOR_VERSION, &gl_minor_version);
    PLOGD << "GL_VERSION                 : " << gl_major_version << "." << gl_minor_version;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);
    PLOGD << "GL_MAX_TEXTURE_SIZE        : " << gl_max_texture_size << " x " << gl_max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &gl_max_texture_image_units);
    PLOGD << "GL_MAX_TEXTURE_IMAGE_UNITS : " << gl_max_texture_image_units;

    dynamic_ubo.quantity(2);

    glsl_data.striding(true);

    render_passes.reserve(10);
    // layers_s.add(&rect);
    // layers_s.quantity(0);
    // static_ubo.add(&layers_s);


    dynamic_ubo.add(&glsl_data);

    auto y= artnet_list_ifaces();

    for (auto ift = y; ift != NULL; ift = artnet_list_ifaces_next(ift)) {

        available_ips.push_back(inet_ntoa(ift->ip_addr.sin_addr));

        PLOGV << "found IP : " << available_ips.back();

    }

    artnet_free_ifaces(y);

    window.max_fps = 59;

    gui = new GUI(window.id);

    // window.keypress_cbs[GLFW_KEY_ESCAPE] = [](int key) { exit(0); };

    // window.keypress_cbs[GLFW_KEY_S] = [](int key) { engine.save(); };

    // window.keypress_cbs[GLFW_KEY_I] = [](int key) { engine.gui->draw_gui = !engine.gui->draw_gui; };

    tree = new Node("tree");


}

Engine::~Engine() { PLOGI << "Engine destroyed"; }

void Engine::init() {

    Callbacks::init();

    Editors::init();



    std::string frag = "#version 430 core\n\nlayout (binding = 0, std140) uniform ubo { int frame, fps, s0, s1; };\n\nuniform sampler2D tex;\n\nin vec2 UV; out vec4 COLOR;\n\nvoid main() { COLOR = texture(tex, UV); if (fps<60)COLOR+=vec4(mod(frame,10)*.05,0,0,1); }";
    std::string vert = "#version 430 core\n\nlayout (binding = 0, std140) uniform ubo { int frame, fps, s0, s1; };\n\nlayout (location = 0) in vec2 POSITION;\nlayout (location = 1) in vec2 TEXCOORD;\n\nout vec2 UV;\n\n\nvoid main() {\n    \n	UV = TEXCOORD;\n    \n	gl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n}";
    shader = new ShaderProgram(frag,vert);

    vbo = new VBO();
    vbo->add(&VBO::quad);


    debug = tree->addOwnr<Debug>()->node()->onrun( [](Node* n) { int fps = std::round(ImGui::GetIO().Framerate); /*n->name_v = ("Debug - " + std::to_string( fps ) + " fps");*/ if (fps<60) { n->color = {1,0,0,1}; }else{ n->color = {1,1,1,1}; } } )->active(true);//->close();
    debug->addPtr<UBO>(&static_ubo)->onchange([](Node* n) { n->is_a<UBO>()->upload(); })->active(false);
    debug->addPtr<UBO>(&dynamic_ubo)->active(false);

    atlas = new Atlas(4096, 4096, "assets/medias/");

    debug->addPtr<Atlas>(atlas);

    // auto comps = debug->addOwnr<Node>("Components")->close();
    // for (auto c : Component::pool) comps->addPtr<Component>(c); // tofix

    models = tree->addOwnr<Node>("Models")->node();
    // models = tree->addFolder<File>("Models", "assets/models/")->node();

    effectors = tree->addOwnr<Node>("Effectors")->node();

    // effectors = tree->addFolder<File>("Effectors", "assets/effectors/")->node();

    // timelines = tree->addOwnr<Node>("Timelines")->node();

    // remaps = tree->addOwnr<Node>("Remaps")->node();

    inputs = tree->addOwnr<Node>("Inputs")->node()->active(1);

    stack = tree->addOwnr<Stack>()->node()->active(1);

    outputs = tree->addOwnr<Node>("Outputs")->node()->active(true);

    debug->select(); // NEEEEEED TO BE ONE SELECTED NODE !

    PLOGI << "Engine initialized";

}

void Engine::run() {

    if (!engine.gui->editors.size()) engine.gui->editors.push_back(new EditorWidget());

    if (!engine.models->childrens.size()) {

        auto quad = engine.models->addPtr<File>( &VBO::quad );


    //  auto x = engine.stack->addOwnr<Layer>();
        // x->add(quad);

    }

    if (!engine.outputs->childrens.size()) {

        auto win = engine.outputs->addPtr<Window>( &engine.window );
        if (engine.stack->childrens.size()) win->get()->layer = engine.stack->childrens[0]->is_a<Layer>();

    }


    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){

        static int frame = 0;
        memcpy(engine.dynamic_ubo.data.data(), &(frame), 4); // aka engine.dynamic_ubo["ENGINE"]["frame"]
        int fps = std::round(ImGui::GetIO().Framerate);
        memcpy(engine.dynamic_ubo.data.data()+4, &fps, 4); // aka engine.dynamic_ubo["ENGINE"]["fps"]
        int alt = frame % 2;
        memcpy(engine.dynamic_ubo.data.data()+8, &alt, 4); // aka engine.dynamic_ubo["ENGINE"]["alt"]

        frame = (frame+1) % 1000;

        static Instance& dynubo2 = Instance(&engine.dynamic_ubo).eq(1).track();

        int offset = 0;
        if (alt) offset = dynubo2.offset;
        engine.dynamic_ubo.upload(engine.dynamic_ubo.data.data(),dynubo2.offset,offset);
        // engine.dynamic_ubo.upload();

        engine.atlas->texture->bind();

        engine.tree->run();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        engine.gui->draw();

    });

}

static void addEffectors(rapidjson::Value &v, Node* node) {

    if (!v.IsObject()) return;

    for (auto &e : v.GetObj()) {

        if (!e.name.IsString() || !e.value.IsString()) continue;

        Node* effector = nullptr;

        for (auto x : engine.effectors->childrens) {

            auto f = x->is_a<File>();

            if (!f) continue;

            if (f->filename() == e.value.GetString()) effector = x;

        }

        if (effector) node->add(effector)->name(e.name.GetString()); else PLOGE << "no effector: " << e.value.GetString();

    }

}


bool isOutput(rapidjson::GenericMember<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>& x) {

    if (!x.name.IsString() || !x.value.IsArray()) return false;

    auto arr = x.value.GetArray();

    if (arr.Size() < 4 || !arr[0].IsInt() || !arr[1].IsInt() || !arr[2].IsInt() || !arr[3].IsInt() ) return false;

    if (arr.Size() > 4 && !arr[4].IsString() ) return false;

    return true;

}


static void if_obj(std::string name, std::function<void(rapidjson::Value::Member&)> cb) { JSON::if_obj_in(name, engine.json.document,cb); }

void Engine::reset() {

    for (auto x : gui->editors) delete x; gui->editors.resize(0);
    for (auto x : stack->childrens) delete x;
    for (auto x : outputs->childrens) delete x;
    for (auto x : inputs->childrens) delete x;

}

struct Prout {};

void Engine::open(const char* file) {

    static std::string json_error = "JSON error";

    reset();

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

    if_obj("models", [&](auto &m) {

        if (m.name.IsString() && m.value.IsString()) auto n = models->addOwnr<File>(m.name.GetString(), m.value.GetString());

    });

    if_obj("effectors", [&](auto &m) {

        if (m.name.IsString() && m.value.IsString()) auto n = effectors->addOwnr<File>(m.name.GetString(), m.value.GetString());
    });

   if_obj("uberlayers", [&](auto &l) {


        if (!l.name.IsString() || !l.value.IsObject()) { PLOGW << json_error; return; }

        auto ubl_ = stack->addOwnr<UberLayer>();
        auto &ubl = *ubl_->get();

        ubl_->name(l.name.GetString());

        auto list = l.value.GetObj();

        for (auto &x : list) {

            if (!x.name.IsString() || !x.value.IsArray()) { PLOGW << json_error; continue; }

            auto info = x.value.GetArray();

            int width = window.width, height = window.height;

            int effectors_id = 2;

            int q = 1;

            if (info.Size() > 1 && info[0].IsInt() && info[1].IsInt()) {

                width = info[0].GetInt(); height = info[1].GetInt();

                if (info.Size() > 2 && info[2].IsInt()) {

                    q = info[2].GetInt() ;

                    if (info.Size() > 3) effectors_id = 3;

                }

                auto &l = ubl.addLayer(width,height);

                auto l_ = ubl_->addPtr<UberLayer::VLayer>(&l);
                l_->active(true)->name(x.name.GetString());

                l.s.quantity(q);

                if (info.Size() > effectors_id && info[effectors_id].IsObject()) {


                    for (auto &e : info[effectors_id].GetObj()) {

                        if (!e.name.IsString() || !e.value.IsString()) { PLOGW <<"weird fx"; continue;}

                        Node* effector = nullptr;


                        engine.effectors->each<File>([&](Node* n, File* f) { if (f->filename() == e.value.GetString()) effector = n; });

                        if (effector) l_->addPtr<Effector>(l.addEffector(effector->is_a<File>()));

                    }

                    engine.stack->trigchange();

                }

            }

        }

        ubl.calc_matrice();

        engine.stack->trigchange();

        ubl.fb.texture->bind(3);

    });


    if_obj("layers", [&](auto &l) {

        if (!l.value.IsArray()) { PLOGW << json_error; return; }

        auto info = l.value.GetArray();

        int width = window.width, height = window.height;

        int models_id = 0;

        if (info.Size() > 2) if (info[0].IsInt() && info[1].IsInt()) {

            width = info[0].GetInt(); height = info[1].GetInt();

            if (info[2].IsObject()) models_id = 2;

        }

        TypedNode<Layer>* layer = stack->addOwnr<Layer>(width,height,l.name.IsString() ? l.name.GetString() : "");

        if (info.Size() <= models_id || !info[models_id].IsObject()) { PLOGW << json_error; return; }

        for (auto &model : info[models_id].GetObj()) {

            if (!model.name.IsString() || !model.value.IsArray()) { PLOGW << json_error; return; }

            auto info = model.value.GetArray();

            if (!info.Size() || !info[0].IsString()) { PLOGW << json_error; return; }

            Node* model_file = nullptr;

            for (Node* x : engine.models->childrens) {

                auto f = x->is_a<File>();

                if (!f) { PLOGW << json_error; return; }

                std::string y = info[0].GetString();

                if (f->filename() == y) model_file = x;

            }

            // for (auto x : engine.models->childrens) if (x->is_a<File>() && x->is_a<File>()->filename == info[0].GetString()) model_file = x;

            if (!model_file) { PLOGE << "no model : " << info[0].GetString(); { PLOGW << json_error; return; } }

            auto new_model = layer->add(model_file)->get<Model>();

            new_model->name(model.name.GetString());

            if (info.Size() < 2 || !info[1].IsInt()) { PLOGW << json_error; return; }

            new_model->get()->s.quantity(info[1].GetInt());

            if (info.Size() < 3 || !info[2].IsObject()) return;

            addEffectors( info[2], new_model->node() );

        }

        addEffectors( info[info.Size()-1], layer->node());

        layer->update();


    });

    if_obj("outputs", [&](auto &m) {

        if (!m.name.IsString() || !m.value.IsObject()) { PLOGW << json_error; return; }

        if (!strcmp(m.name.GetString(),"ndi")) for (auto &x : m.value.GetObj()) {

            if (!isOutput(x))  { PLOGW << json_error; return; }

            auto arr = x.value.GetArray();
            if (arr.Size() < 5) { PLOGW << json_error; return; }
            // if (arr.Size() == 4) { PLOGW << json_error; return; }


            Node* layer = nullptr;
            if (arr.Size()>4) layer = engine.stack->child(arr[4].GetString());

            Node* n = engine.outputs->addOwnr<NDI::Sender>( arr[0].GetInt() , arr[1].GetInt(), x.name.GetString(), (layer?layer->is_a<Layer>():nullptr))->active(false);

            if (layer) layer->referings.insert( n );

        }

        if (!strcmp(m.name.GetString(),"monitor")) for (auto &x : m.value.GetObj()) {

            if (!isOutput(x))  { PLOGW << json_error; return; }

            auto arr = x.value.GetArray();

            Node* layer = nullptr;
            if (arr.Size()>4) layer = engine.stack->child(arr[4].GetString());
            auto window = engine.outputs->addPtr<Window>( &engine.window );

            window->name(x.name.GetString());
            window->get()->layer = (layer?layer->is_a<Layer>():nullptr);

            engine.window.size( arr[0].GetInt() , arr[1].GetInt() );
            engine.window.pos( arr[2].GetInt() , arr[3].GetInt() );

            break; // only one alloweed for nowe

        }

    });


    if_obj("inputs", [&](auto &m) {

        if (!m.name.IsString() || !m.value.IsObject()) { PLOGW << json_error; return; }

        if (!strcmp(m.name.GetString(),"artnet")) for (auto &x : m.value.GetObj()) {

            if (!x.name.IsString()) { PLOGW << json_error; continue; }

            if (x.value.IsString()) engine.inputs->addOwnr<Artnet>( x.value.IsString() ? x.value.GetString() : "" )->active(1)->name( x.name.GetString() );

            if (!x.value.IsArray()) { PLOGW << json_error; continue; }

            auto arr = x.value.GetArray();

            if (!arr.Size()) { PLOGW << json_error; continue; }

            TypedNode<Artnet>* an_ = engine.inputs->addOwnr<Artnet>( arr[0].IsString() ? arr[0].GetString() : "" );

            an_->active(1)->name( x.name.GetString() );

            auto &an = *an_->get();

            if (arr.Size() < 2 || !arr[1].IsObject()) { PLOGW << json_error; continue; }

            for (auto &remap : arr[1].GetObj()) {

                if (!remap.name.IsString() || !remap.value.IsArray()) { PLOGW << json_error; continue; }

                auto arr = remap.value.GetArray();
                int x = arr.Size();

                if (arr.Size() < 3 || !arr[0].IsInt() || !arr[1].IsInt() || !arr[2].IsString()) { PLOGW << json_error; continue; }

                // should find (Layer/Model/Effector aka Effectable) node(aka "w" down there) (then get Member path)

                Instance inst(&engine.dynamic_ubo);

                bool continue_ = false;
                for (auto name : split(arr[2].GetString())) {

                        inst = inst[name];

                        if (!inst.def()) {

                            PLOGW << json_error;
                             continue_ = true;
                             break;

                        }
                }
                if (continue_) continue;

                if (inst.def() == inst.buff) { PLOGW << json_error; continue; }

                auto &uni = *an.universe(arr[0].GetInt()).instances[0].get();

                std::vector<DMXRemap::Attribute> attrs;
                if ( arr.Size() > 3 && arr[3].IsArray() ) for (auto &x : arr[3].GetArray()) if (x.IsInt()) attrs.push_back({x.GetInt()});

                int q = 1;
                if ( arr.Size() > 4 && arr[4].IsInt() ) q = arr[4].GetInt();

                DMXRemap* dmxremap = new DMXRemap(&uni, &inst.track(), arr[1].GetInt()-1, attrs, q);

                uni.remaps.push_back( dmxremap );

                auto n = an_->addPtr<DMXRemap>(dmxremap)->name(remap.name.GetString());

                std::string sss =arr[2].GetString() ;
                auto w = engine.tree->child(sss);
                if (!w) { PLOGE <<arr[2].GetString()<< " not found"; return; }
                w->referings.insert( n );

            }

        }

    });

    if (json.document.HasMember("editors") && json.document["editors"].IsArray()) for (auto &e : json.document["editors"].GetArray()) {

        if (!e.IsArray()) { PLOGW << json_error; continue; }

        auto x = e.GetArray();

        if (e.Size()<5) { PLOGW << json_error; continue; }

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

    if (!strlen(file)) { PLOGE << "No project file"; return;}


    if (!json.document.HasMember("editors")) json.document.AddMember("editors", rapidjson::Value(rapidjson::kArrayType), json.document.GetAllocator());
    if (!json.document.HasMember("models")) json.document.AddMember("models", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());
    if (!json.document.HasMember("effectors")) json.document.AddMember("effectors", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());
    if (!json.document.HasMember("layers")) json.document.AddMember("layers", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());
    if (!json.document.HasMember("uberlayers")) json.document.AddMember("uberlayers", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());
    if (!json.document.HasMember("outputs")) json.document.AddMember("outputs", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());
    if (!json.document.HasMember("remaps")) json.document.AddMember("remaps", rapidjson::Value(rapidjson::kObjectType), json.document.GetAllocator());

    json.document["editors"].Clear();

    for (auto e : gui->editors) {

        auto v = rapidjson::Value(rapidjson::kArrayType);

        v.PushBack(0, json.document.GetAllocator());
        v.PushBack(0, json.document.GetAllocator());
        v.PushBack(0, json.document.GetAllocator());
        v.PushBack(0, json.document.GetAllocator());

        if (e->selected) v.PushBack(rapidjson::Value(e->selected->nameSTL().c_str(), json.document.GetAllocator()), json.document.GetAllocator());
        if (e->locked) v.PushBack(rapidjson::Value(true), json.document.GetAllocator());

        auto &x = json.document["editors"].PushBack(v, json.document.GetAllocator());

    }


    json.document["models"].RemoveAllMembers();
    for (auto m : models->childrens) { json.document["models"].AddMember(rapidjson::Value(m->is_a<File>()->filename().c_str(), json.document.GetAllocator()), rapidjson::Value(&m->is_a<File>()->data[0], json.document.GetAllocator()), json.document.GetAllocator()); }

    json.document["effectors"].RemoveAllMembers();
    for (auto m : effectors->childrens) {

        json.document["effectors"].AddMember(rapidjson::Value(m->is_a<File>()->filename().c_str(), json.document.GetAllocator()), rapidjson::Value(&m->is_a<File>()->data[0], json.document.GetAllocator()), json.document.GetAllocator());

    }

    rapidjson::Document::AllocatorType& allocator = json.document.GetAllocator();

    json.document["layers"].RemoveAllMembers();

    stack->each<Layer>([&](Node* node, Layer* layer){

        auto &arr = json.document["layers"];

        auto  lay = rapidjson::Value(rapidjson::kArrayType);

        lay.PushBack(layer->fb.width, allocator);
        lay.PushBack(layer->fb.height, allocator);

        auto  models = rapidjson::Value(rapidjson::kObjectType);

        for (auto model : layer->models) {

            auto new_model = rapidjson::Value(rapidjson::kArrayType);

            new_model.PushBack(rapidjson::Value(model.get()->file->filename().c_str(), allocator), allocator);

            new_model.PushBack(model.get()->s.quantity(),allocator);

            auto effects = rapidjson::Value(rapidjson::kObjectType);
            for (auto e : model.get()->effectors) effects.AddMember( rapidjson::Value(e.get()->ref.name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator );
            new_model.PushBack( effects, allocator );

            models.AddMember(rapidjson::Value(model.get()->s.name().c_str(), allocator), new_model, allocator);

        }

        lay.PushBack(models, allocator);

        auto effects = rapidjson::Value(rapidjson::kObjectType);
        for (auto e : layer->effectors) effects.AddMember( rapidjson::Value(e.get()->ref.name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator );
        lay.PushBack( effects, allocator );

        arr.AddMember( rapidjson::Value(node->name().c_str(), allocator)  , lay, allocator );

    });


    json.document["uberlayers"].RemoveAllMembers();

    stack->each<UberLayer>([&](Node* node, UberLayer* ubl){

        auto &arr = json.document["uberlayers"];

        auto  ubl_ = rapidjson::Value(rapidjson::kObjectType);

        for (auto layer : ubl->layers) {

            auto  layer_ = rapidjson::Value(rapidjson::kArrayType);

            layer_.PushBack(layer.get()->w, allocator);
            layer_.PushBack(layer.get()->h, allocator);
            layer_.PushBack(layer.get()->s.quantity(), allocator);

            auto  effectors_ = rapidjson::Value(rapidjson::kObjectType);
            for (auto e : layer->effectors) effectors_.AddMember( rapidjson::Value(e.get()->ref.name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator );
            layer_.PushBack(effectors_, allocator);

            ubl_.AddMember(rapidjson::Value(layer.get()->s.name().c_str(), allocator)  , layer_, allocator);

        }

        arr.AddMember(rapidjson::Value(node->name().c_str(), allocator), ubl_, allocator);

    });

    // json.document["inputs"].SetObject();
    // json.document["inputs"].RemoveAllMembers();
    // for (auto input : inputs->childrens) {

    // }

    json.document["outputs"].SetObject();
    json.document["outputs"].RemoveAllMembers();
    for (auto output : outputs->childrens) {

        auto  outputarr = rapidjson::Value(rapidjson::kArrayType);

        auto output_ = (Output*)output->ptr_(); // big leap

        outputarr.PushBack( output_->width, allocator );
        outputarr.PushBack( output_->height, allocator );
        outputarr.PushBack( output_->offset_x, allocator );
        outputarr.PushBack( output_->offset_y, allocator );
        if (output_->layer) outputarr.PushBack( rapidjson::Value(output_->layer->s.name().c_str(), allocator ), allocator );

        // if ( output->is_a<NDI::Sender>() ) { }

        auto &outputs = json.document["outputs"];

        if ( output->is_a_nowarning<Window>() ) {

            if (!outputs.HasMember("monitor")) outputs.AddMember(rapidjson::Value("monitor", allocator) , rapidjson::Value(rapidjson::kObjectType), allocator);

            outputs["monitor"].AddMember( rapidjson::Value(output->name().c_str(), allocator)  , outputarr, allocator );

        }
        if ( output->is_a_nowarning<NDI::Sender>() ) {

            if (!outputs.HasMember("ndi")) outputs.AddMember(rapidjson::Value("ndi", allocator) , rapidjson::Value(rapidjson::kObjectType), allocator);

            outputs["ndi"].AddMember( rapidjson::Value(output->name().c_str(), allocator)  , outputarr, allocator );

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
