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
#include "ndi.hpp"
#include "json.hpp"
#include "open.hpp"

#include "callbacks.hpp"



Engine::Engine(uint16_t width, uint16_t height) : window(1,1,0,0), dynamic_ubo("dynamic_ubo"), static_ubo("static_ubo") {

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


    dynamic_ubo.add(&glsl_data);


    window.max_fps = 59;

    gui = new GUI(window.id);

    // window.keypress_cbs[GLFW_KEY_ESCAPE] = [](int key) { exit(0); };

    // window.keypress_cbs[GLFW_KEY_S] = [](int key) { engine.save(); };

    // window.keypress_cbs[GLFW_KEY_I] = [](int key) { engine.gui->draw_gui = !engine.gui->draw_gui; };

    tree = new Node("tree");


}

Engine::~Engine() {

    for (auto x : Struct::owned) {
       delete x;
    }

    PLOGI << "Engine destroyed";

}

void Engine::init() {

    #ifdef ROCH
    auto old_sev = logger.appender.max_severity;
    logger.cout(Sev::info); 
    PLOGI << "hello Roche ! ";
    logger.cout(old_sev);
    #endif

    Callbacks::init();

    Editors::init();

    debug = tree->addOwnr<Debug>()->node()->onrun( [](Node* n) { int fps = std::round(ImGui::GetIO().Framerate); /*n->name_v = ("Debug - " + std::to_string( fps ) + " fps");*/ if (fps<60) { n->color = {1,0,0,1}; }else{ n->color = {1,1,1,1}; } } )->active(true);//->close();
    debug->addPtr<UBO>(&static_ubo)->onchange([](Node* n) { n->is_a<UBO>()->upload(); })->active(false);
    debug->addPtr<UBO>(&dynamic_ubo)->active(false);
    
    medias = tree->addOwnr<Node>("Medias")->node();

    // auto comps = debug->addOwnr<Node>("Components")->close();
    // for (auto c : Component::pool) comps->addPtr<Component>(c); // tofix

    models = tree->addOwnr<Node>("Models")->node();
    // models = tree->addFolder<File>("Models", "assets/models/")->node();

    effectors = tree->addOwnr<Node>("Effectors")->node();

    auto feedbackeffector = new FeedbackEffector();
    engine.effectors->addPtr<Effector>(feedbackeffector);

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
        // if (engine.stack->childrens.size()) win->get()->layer = engine.stack->childrens[0]->is_a<Layer>();

    }


    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        engine.gui->draw();

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


        engine.tree->run();

    });

}




void Engine::reset() {

    for (auto x : gui->editors) delete x; gui->editors.resize(0);
    for (auto x : stack->childrens) delete x;
    for (auto x : outputs->childrens) delete x;
    for (auto x : inputs->childrens) delete x;
    for (auto x : medias->childrens) delete x;

}

struct Prout {};

void Engine::open(const char* file) {

    Open o;
    
    o.json(file);

}

void Engine::save() { save(project_filepath.c_str()); }

void Engine::save(const char* file) {

    if (!strlen(file)) { PLOGE << "No project file"; return;}

    JSON json;

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

            new_model.PushBack(model.get()->s_->quantity(),allocator);

            auto effects = rapidjson::Value(rapidjson::kObjectType);
            // for (auto e : model.get()->effectors) effects.AddMember( rapidjson::Value(e.get()->ref().name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator ); // TODOTODO
            new_model.PushBack( effects, allocator );

            models.AddMember(rapidjson::Value(model.get()->s_->name().c_str(), allocator), new_model, allocator);

        }

        lay.PushBack(models, allocator);

        auto effects = rapidjson::Value(rapidjson::kObjectType);
        // for (auto e : layer->effectors) effects.AddMember( rapidjson::Value(e.get()->ref().name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator );// TODOTODO
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
            layer_.PushBack(layer.get()->s_->quantity(), allocator);

            auto  effectors_ = rapidjson::Value(rapidjson::kObjectType);
            // for (auto e : layer->effectors) effectors_.AddMember( rapidjson::Value(e.get()->ref().name().c_str(), allocator), rapidjson::Value(e.get()->file->filename().c_str(), allocator), allocator );// TODOTODO
            layer_.PushBack(effectors_, allocator);

            ubl_.AddMember(rapidjson::Value(layer.get()->s_->name().c_str(), allocator)  , layer_, allocator);

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
        if (output_->fb) {

            Layer* lay = nullptr;

            stack->each<Layer>([&](Node* node, Layer* layer){ if (&layer->fb == output_->fb) lay = layer; });

            outputarr.PushBack( rapidjson::Value(lay->s_->name().c_str(), allocator ), allocator );

        }

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
