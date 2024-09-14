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

#include <GLFW/glfw3.h>
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

    

    static auto exit_cb = []() { exit(0); };

    window.keypress_cbs[{GLFW_KEY_LEFT_CONTROL, GLFW_KEY_ESCAPE}] = exit_cb;
    window.keypress_cbs[{GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_ESCAPE}] = exit_cb;
    window.keypress_cbs[{GLFW_KEY_LEFT_SHIFT, GLFW_KEY_ESCAPE}] = exit_cb;
    window.keypress_cbs[{GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_ESCAPE}] = exit_cb;

    static auto save_cb = [&]() { save(); };

    window.keypress_cbs[{GLFW_KEY_LEFT_CONTROL, GLFW_KEY_S}] = save_cb;
    window.keypress_cbs[{GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_S}] = save_cb;
    window.keypress_cbs[{GLFW_KEY_LEFT_SHIFT, GLFW_KEY_S}] = save_cb;
    window.keypress_cbs[{GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_S}] = save_cb;

    static auto guiact_cb = [&]() { 

        window.end_of_render_cbs.emplace_back(std::pair<void*,std::shared_ptr<std::function<void(void*)>>>{nullptr, std::make_shared<std::function<void(void*)>>([&](void* ptr) { 
            
            engine.gui(!engine.gui_v);  
            
        })});
        
    };

    window.keypress_cbs[{GLFW_KEY_LEFT_CONTROL, GLFW_KEY_I}] = guiact_cb;
    window.keypress_cbs[{GLFW_KEY_RIGHT_CONTROL, GLFW_KEY_I}] = guiact_cb;
    window.keypress_cbs[{GLFW_KEY_LEFT_SHIFT, GLFW_KEY_I}] = guiact_cb;
    window.keypress_cbs[{GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_I}] = guiact_cb;
    window.mousedown_cb = [](int button) { 
        
        if (button == GLFW_MOUSE_BUTTON_LEFT) guiact_cb(); 
        else if (button == GLFW_MOUSE_BUTTON_RIGHT) exit_cb();
        
    };

    tree = new Node("tree");

    window.keypress();
    // gui(true);

    // gui(false);
    // gui(true);

    // gui(false);
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

    models = tree->addOwnr<Node>()->name("Models")->active(false);

    effectors = tree->addOwnr<Node>()->name("Effectors")->active(false);

    inputs = tree->addOwnr<Node>()->name("Inputs")->active(1);

    stack = tree->addOwnr<Stack>()->active(1)->allow<DrawCall>();

    outputs = tree->addOwnr<Node>()->name("Outputs")->active(true);

    PLOGI << "Engine initialized";
    PLOGV << "===========================";

}
void Engine::gui(bool active) {

    if (gui_v && !active) {

        auto gui_v_ = gui_v;
        gui_v = nullptr;
        delete gui_v_;

    }else if (!gui_v && active) {

        gui_v = new GUI(&window);
        gui_v->trees[0]->selected= engine.tree;

    }

}

void Engine::run() {

    if (models && !models->childrens.size()) 
        auto quad = models->addPtr<File>( &VBO::quad );

    if (outputs && !outputs->childrens.size()) {
        
        auto win = outputs->addPtr<Window>( &window );

        if (stack->childrens.size()) 
        
            for (auto output_ : outputs->childrens) {
                
                auto output = output_->is_a<Output>();

                if (output && !output->fb) 

                    win->add(stack->childrens[0]);

            }
        
    }

    window.fit();
    window.size(50,50);

    window.render([&](){

        if (gui_v)
        
            gui_v->draw();
        
        if (dynamic_ubo) {   
            
            static int frame = 0;
            memcpy(dynamic_ubo->data(), &(frame), 4); // aka dynamic_ubo["ENGINE"]["frame"]
            int fps = std::ceil(window.fps.fps);
            memcpy(dynamic_ubo->data()+4, &fps, 4); // aka dynamic_ubo["ENGINE"]["fps"]
            int alt = frame % 2;
            memcpy(dynamic_ubo->data()+8, &alt, 4); // aka dynamic_ubo["ENGINE"]["alt"]

            frame++;
            
            int dynubofp = dynamic_ubo->footprint();

            int from = 0;//alt ? dynubofp : 0 ;

            int size = dynubofp;

            int to = from+size;
            
            dynamic_ubo->upload(dynamic_ubo->data()+from, to, size);
            
        }
        
        tree->run();

        for (auto cb : end_of_render_cbs)
            (*cb.get())();

        end_of_render_cbs.clear();

    });

}




void Engine::reset() {

    gui(false);
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
