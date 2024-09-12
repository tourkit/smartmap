#include "engine.hpp"
#include "drawcall.hpp"
#include "layer.hpp"
#include "struct.hpp"
#include "effector.hpp"
#include "gui.hpp"
#include "editor.hpp"
#include "gui.hpp"
#include "ubo.hpp"
#include "node.hpp"
#include "utils.hpp"
#include "vbo.hpp"
#include "instance.hpp"
#include "open.hpp"
#include "save.hpp"

#include "callbacks.hpp"
#include "editors.hpp"
#include "tree.hpp"

#include <cmath>


Engine::Engine(uint16_t width, uint16_t height) : window(1,1,0,0) {

    glGetIntegerv(GL_MAJOR_VERSION, &gl_major_version);
    glGetIntegerv(GL_MINOR_VERSION, &gl_minor_version);
    PLOGD << "GL_VERSION                 : " << gl_major_version << "." << gl_minor_version;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);
    PLOGD << "GL_MAX_TEXTURE_SIZE        : " << gl_max_texture_size << " x " << gl_max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &gl_max_texture_image_units);
    PLOGD << "GL_MAX_TEXTURE_IMAGE_UNITS : " << gl_max_texture_image_units;


    glsl_data.striding(true);

    render_passes.reserve(10);

    window.max_fps = 59;

    gui = new GUI(window.id);

    // window.keypress_cbs[GLFW_KEY_ESCAPE] = [](int key) { exit(0); };

    // window.keypress_cbs[GLFW_KEY_S] = [](int key) { engine.save(); };

    // window.keypress_cbs[GLFW_KEY_I] = [](int key) { engine.gui->draw_gui = !engine.gui->draw_gui; };

    tree = new Node("tree");

    gui->trees[0]->selected = tree;

    NODE<Node>::onadd<AnyNode>([](Node*_this,Node*node){ return node; });

    tree->active(1);


}

Engine::~Engine() {

    for (auto x : Struct::owned) {
       delete x;
    }

    if (dynamic_ubo) delete dynamic_ubo;
    if (static_ubo) delete static_ubo;

    PLOGI << "Engine destroyed";

}

void Engine::init() {

    Callbacks::init();

    Editors::init();
    
    dynamic_ubo = new UBO("dynamic_ubo");
    static_ubo = new UBO("static_ubo");
    
    dynamic_ubo->quantity(2);
    dynamic_ubo->add(&glsl_data);

    auto* debug_ = tree->addOwnr<Debug>()->allow<AnyNode>();
    debug = debug_;
    debug->on(Node::RUN, [](Node* n) { int fps = std::round(ImGui::GetIO().Framerate); /*n->name_v = ("Debug - " + std::to_string( fps ) + " fps");*/ if (fps<60) { n->color = {1,0,0,1}; }else{ n->color = {1,1,1,1}; } } )->active(false);//->close();
    debug->addPtr<UBO>(static_ubo)->on(Node::CHANGE, [](Node* n) { 
        n->is_a<UBO>()->upload(); 
    })->active(false);
    debug->addPtr<UBO>(dynamic_ubo)->active(false);
    
    medias = tree->addOwnr<Node>()->name("Medias")->active(false);

    // auto comps = debug->addOwnr<Node>()->name("Components")->close();
    // for (auto c : Component::pool) comps->addPtr<Component>(c); // tofix

    models = tree->addOwnr<Node>()->name("Models")->active(false);
    // models = tree->addFolder<File>("Models", "assets/models/");

    effectors = tree->addOwnr<Node>()->name("Effectors")->active(false);

    // effectors = tree->addFolder<File>("Effectors", "assets/effectors/");

    // timelines = tree->addOwnr<Node>()->name("Timelines");

    // remaps = tree->addOwnr<Node>()->name("Remaps");

    inputs = tree->addOwnr<Node>()->name("Inputs")->active(1);

    stack = tree->addOwnr<Stack>()->active(1)->allow<DrawCall>();

    outputs = tree->addOwnr<Node>()->name("Outputs")->active(true);

    PLOGI << "Engine initialized";

}

void Engine::run() {

    if (!gui->editors.size()) 
        gui->editors.push_back(new EditorWidget(gui));

    if (models && !models->childrens.size()) {

        auto quad = models->addPtr<File>( &VBO::quad );


    //  auto x = stack->addOwnr<Layer>();
        // x->add(quad);

    }

    if (outputs && !outputs->childrens.size()) {
        
        auto win = outputs->addPtr<Window>( &window );

        if (stack->childrens.size()) 
        
            for (auto output_ : outputs->childrens) {
                
                auto output = output_->is_a<Output>();

                if (output && !output->fb) 

                    win->add(stack->childrens[0]);

            }
        
    }


    if (!gui->selected) 
        gui->selected = debug;

    auto &window = getInstance().window;
    

    while (!glfwWindowShouldClose(window.id)) {

    // PLOGW << "begin frame";

        window.render([](){

            engine.gui->draw();
            
            if (engine.dynamic_ubo) {        
                static int frame = 0;
                memcpy(engine.dynamic_ubo->data(), &(frame), 4); // aka engine.dynamic_ubo["ENGINE"]["frame"]
                int fps = std::round(ImGui::GetIO().Framerate);
                memcpy(engine.dynamic_ubo->data()+4, &fps, 4); // aka engine.dynamic_ubo["ENGINE"]["fps"]
                int alt = frame % 2;
                memcpy(engine.dynamic_ubo->data()+8, &alt, 4); // aka engine.dynamic_ubo["ENGINE"]["alt"]

                frame = (frame+1) % 65536;//window.displays.back().rate;
                
                int glsldatafp = glsl_data.footprint();
                int dynubofp = engine.dynamic_ubo->footprint();
                
                engine.dynamic_ubo->upload(engine.dynamic_ubo->data(),alt?glsldatafp:dynubofp);

                if (alt) 
                    engine.dynamic_ubo->upload(engine.dynamic_ubo->data()+glsldatafp,dynubofp-glsldatafp,dynubofp+glsldatafp);
                
            }
            
            engine.tree->run();

        });

    
    // PLOGW << "begin frame";
}
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
