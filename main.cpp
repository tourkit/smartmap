#include "src/window.hpp"
#include "src/gui.hpp" 
#include "new/ecs.hpp" 

// #include "src/gui/ShadertoyWindow.h" 
// #include "src/gui/TreeviewWindow.h" 
// #include "src/renderer.hpp" 
 
auto* window = new GL::Window(false,800,400,1120); 
auto* gui = new GUI{window->window}; 
flecs::world ecs;


struct Object { };
struct Color { float value[3] = {255,255,255}; };
struct Drawcall {

    // I -
    //gl VAO , then VAO *ing to Mesh
    //gl ShaderProgram

    // II - 
    //gl UBO

    // III -
    //gl inBuffers
    //gl OutBuffer


};

int main() {

    ecs.component<ImGuiStyle>();

    ecs.entity("test").set<Color>({255,0,0});
    auto q = ecs.filter_builder<>().term(ecs.component("Color")).build();
    q.each([](flecs::entity e) { std::cout << e.str() << std::endl; });

    while(true) window->render([&]() { 
        
        
    
    });

} 

