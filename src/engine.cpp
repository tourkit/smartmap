#include "engine.hpp"
#include "struct.hpp"
#include "effector.hpp"
#include "gui.hpp"
#include "editor.hpp"
#include "gui.hpp"
#include "ubo.hpp"
#include "node.hpp"
#include "atlas.hpp"
#include "vbo.hpp"
#include "instance.hpp"
#include "open.hpp"
#include "save.hpp"

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
    tree->active(1);


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

    debug = tree->addOwnr<Debug>()->node()->onrun( [](Node* n) { int fps = std::round(ImGui::GetIO().Framerate); /*n->name_v = ("Debug - " + std::to_string( fps ) + " fps");*/ if (fps<60) { n->color = {1,0,0,1}; }else{ n->color = {1,1,1,1}; } } )->active(false);//->close();
    debug->addPtr<UBO>(&static_ubo)->onchange([](Node* n) { n->is_a<UBO>()->upload(); })->active(false);
    debug->addPtr<UBO>(&dynamic_ubo)->active(false);
    
    medias = tree->addOwnr<Node>("Medias")->active(false)->node();

    // auto comps = debug->addOwnr<Node>("Components")->close();
    // for (auto c : Component::pool) comps->addPtr<Component>(c); // tofix

    models = tree->addOwnr<Node>("Models")->active(false)->node();
    // models = tree->addFolder<File>("Models", "assets/models/")->node();

    effectors = tree->addOwnr<Node>("Effectors")->active(false)->node();

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

void Engine::open(const char* file) {

    project_filepath  = file;

    Open o;
    
    o.json(file);

}

void Engine::save() { save(project_filepath.c_str()); }

void Engine::save(const char* file) {

    if (!strlen(file)) { PLOGE << "No project file"; return;}

    Save save;

    save.json(file);

}
