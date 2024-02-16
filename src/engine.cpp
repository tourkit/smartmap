#include "engine.hpp"
#include "directory.hpp"
#include "struct.hpp"
#include "shaderfx.hpp"
#include "drawcall.hpp"
#include "gui.hpp"
#include "editors.hpp"
#include "file.hpp"


#include "nodes.hpp"

Engine::Engine(uint16_t width, uint16_t height) 
    : window(1920,1080,2560,0), 
     gui(new GUI(window.id)) {
    window.max_fps = 59;

    dynamic_ubo = new UBO("dynamic_ubo");
    static_ubo = new UBO("static_ubo");

    PLOGD << "Engine created";

}

Engine::~Engine() { PLOGD << "Engine destroyed"; }
 
void Engine::run() {

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){
        
        auto &engine = Engine::getInstance();

        // engine.stack->run(); // better be : tree.run(); ( owning gui and all..)
        engine.tree.run(); 

        engine.gui->draw(); 

    });

};


struct DC {};

void Engine::init() {

    Nodes::init();
    
    Editors::init();


    auto vbo = tree.addOwnr<VBO>()->get();
    vbo->import(new File("assets/model/quad.obj")); // fuck owning

    auto frag = tree.addOwnr<File>("assets/shader/test.frag");
    auto vert = tree.addOwnr<File>("assets/shader/test.vert");

    auto shader_ = tree.addOwnr<ShaderProgram>(frag->get()->data, vert->get()->data);
    shader_->refering = frag->node();
    auto shader = shader_->get();
    auto cb = [shader,frag,vert](Node* node){ 
        
        shader->create(frag->get()->data, vert->get()->data); 
        
    };

    frag->onchange(cb);
    // vert->onchange(cb);

    dynamic_ubo = tree.addOwnr<UBO>("dynamic_ubo")->get();
    dynamic_ubo->buffer.addObj(new Struct("test",{"float","float","float","float"}))->push();
    dynamic_ubo->subscribers.push_back(shader);

    auto dc = tree.addOwnr<DC>()->get();

    NODE<DC>::onrun([shader,vbo](Node* node, DC* dc) {
        
        shader->use();
        vbo->draw();
         
    });



    // stack->addPtr<UBO>(dynamic_ubo)->select();

    // tree.addPtr<UBO>(static_ubo);

    // auto models = tree.addOwnr<Directory>("assets/model/");
    // auto shaders = tree.addOwnr<Directory>("assets/shaders/");


    // auto dc = stack->addOwnr<DrawCall>();

    // auto model = dc->addPtr(models->childrens[0]); 
    // model->addPtr(shaders->childrens[0]); 







    
    // Node* controllers = tree->add(new Node{"Controllers"});
    // auto an = controllers->add(new Node{"Art-Net"});
    // an->add(new Node{"1"});
    // an->add(new Node{"2"});
    // an->add(new Node{"3"});
    // an->add(new Node{"4"});

    // atlas = (Atlas*)tree->add(new Atlas(4096, 4096, "assets/media/"));
    

     PLOGD << "Engine initialized";

}