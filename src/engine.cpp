#include "engine.hpp"
#include "layer.hpp"
#include "struct.hpp"
#include "effector.hpp"
#include "gui.hpp"
#include "editor.hpp"
#include "gui.hpp"
#include "ubo.hpp"
#include "node.hpp"
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

    Callbacks::init();

    Editors::init();
    

    
    auto* debug_ = tree->addOwnr<Debug>();
    debug = debug_;
    debug->on(Node::RUN, [](Node* n) { int fps = std::round(ImGui::GetIO().Framerate); /*n->name_v = ("Debug - " + std::to_string( fps ) + " fps");*/ if (fps<60) { n->color = {1,0,0,1}; }else{ n->color = {1,1,1,1}; } } )->active(false);//->close();
    debug->addPtr<UBO>(&static_ubo)->on(Node::CHANGE, [](Node* n) { 
        n->is_a<UBO>()->upload(); 
    })->active(false);
    debug->addPtr<UBO>(&dynamic_ubo)->active(false);
    
    medias = tree->addOwnr<Node>()->name("Medias")->active(false);

    // auto comps = debug->addOwnr<Node>()->name("Components")->close();
    // for (auto c : Component::pool) comps->addPtr<Component>(c); // tofix

    models = tree->addOwnr<Node>()->name("Models")->active(false);
    // models = tree->addFolder<File>("Models", "assets/models/");

    effectors = tree->addOwnr<Node>()->name("Effectors")->active(false);

    auto feedbackeffector = new FeedbackEffector();
    engine.effectors->addPtr<Effector>(feedbackeffector);

    // effectors = tree->addFolder<File>("Effectors", "assets/effectors/");

    // timelines = tree->addOwnr<Node>()->name("Timelines");

    // remaps = tree->addOwnr<Node>()->name("Remaps");

    inputs = tree->addOwnr<Node>()->name("Inputs")->active(1);

    stack = tree->addOwnr<Stack>()->active(1);

    outputs = tree->addOwnr<Node>()->name("Outputs")->active(true);

    PLOGI << "Engine initialized";

}

void Engine::run() {

    if (!engine.gui->editors.size()) engine.gui->editors.push_back(new EditorWidget());

    if (!engine.models->childrens.size()) {

        auto quad = engine.models->addPtr<File>( &VBO::quad );


    //  auto x = engine.stack->addOwnr<Layer>();
        // x->add(quad);

    }

    if (!engine.outputs->childrens.size()) engine.outputs->addPtr<Window>( &engine.window );

    for (auto output_ : engine.outputs->childrens) {
        
        auto output = output_->is_a<Output>();

        if (output && !output->fb && engine.stack->childrens.size()) {
            
            output->fb = &engine.stack->childrens[0]->is_a<Layer>()->fb;
            
            engine.stack->childrens[0]->referings.insert(output_);
        
        }

    }

    if (!Node::selected) Node::selected = debug;

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) 
    
        window.render([](){

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
