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


    PLOGD << "Engine created";

}

Engine::~Engine() { PLOGD << "Engine destroyed"; }
 
void Engine::run() {

    auto &window = getInstance().window;

    while (!glfwWindowShouldClose(window.id)) window.render([](){
        
        auto &engine = Engine::getInstance();

        // engine.stack->run(); // better be : tree->run(); ( owning gui and all..)
        engine.tree->run(); 

        engine.gui->draw(); 

    });

};


struct DC {};

void Engine::init() {

    Nodes::init();
    
    Editors::init();

    dynamic_ubo = new UBO("dynamic_ubo");
    static_ubo = new UBO("static_ubo");
    
    auto ubo = stack->addPtr<UBO>(dynamic_ubo);
    ubo->select();

    // tree->addPtr<UBO>(static_ubo);

    
    
    
    auto models = tree->addOwnr<Directory>("assets/model/");
    auto shaders = tree->addOwnr<Directory>("assets/shaders/");
    
    auto dc = stack->addOwnr<DrawCall>();
    
    auto model = dc->addPtr(models->childrens[0]); 

    model->addPtr(shaders->childrens[0]); 
    // dc->active = false;
 
    // dc->get()->vbo.import(new File("C:/users/root/cpp/smartmap/assets/model/quad.obj")); // fuck owning

    auto obj = dynamic_ubo->addObj(new Struct("test",{"RGBA"}));
    obj->push();

    auto frag = tree->addOwnr<File>("C:/users/root/cpp/smartmap/assets/shader/test.frag");
    auto vert = tree->addOwnr<File>("C:/users/root/cpp/smartmap/assets/shader/test.vert");

    // dc->get()->shader.create(frag->get()->data, vert->get()->data);

    dynamic_ubo->subscribers.push_back(&dc->get()->shader);

    // auto dc1 = tree->addOwnr<DC>()->get();

    NODE<DC>::onrun([dc](Node* node, DC* Dc) {
        
        dc->get()->shader.use();
        dc->get()->vbo.draw();

    });

    



    // Node* controllers = tree->add(new Node{"Controllers"});
    // auto an = controllers->add(new Node{"Art-Net"});
    // an->add(new Node{"1"});
    // an->add(new Node{"2"});
    // an->add(new Node{"3"});
    // an->add(new Node{"4"});

    // atlas = (Atlas*)tree->add(new Atlas(4096, 4096, "assets/media/"));
    

     PLOGD << "Engine initialized";

}