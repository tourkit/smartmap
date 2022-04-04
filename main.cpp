// add buttons/menu to title bar
//pickable item
// icon for item



// auto adjust table

// pool of object singeltons

#include "src/window.hpp"
#include "src/gui.hpp" 
#include "include/vendor/flecs/flecs.h"

// #include "src/gui/ShadertoyWindow.h" 
// #include "src/gui/TreeviewWindow.h" 
// #include "src/renderer.hpp" 
 
auto* window = new GL::Window(false,800,400,1120); 

auto* gui = new GUI{window->window}; 

flecs::world ecs;



struct Rerender {

    float x;
    float y;

};

struct Nonode { float BGcolor[3] = {255,255,255}; };

template<typename T>
struct Pool { 

    static inline std::vector<T*> pool; 
    
    int pool_id = 0;

    Pool() { pool_id = pool.size(); pool.push_back(this); }

};


struct TEST_GUI : GUIRenderer {
    
    TEST_GUI() {

    }

    void draw() override { 

        ImGui::ShowDemoWindow();

        ImGui::Begin("Tree");

        ecs.each([](flecs::entity e, Nonode& node) {  

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(node.BGcolor[0],node.BGcolor[1],node.BGcolor[2],255));
            ImGui::Text(e.name().c_str());
            ImGui::PopStyleColor();

        });

        ImGui::End();

     }

} test_gui;


    auto renderers = ecs.prefab("Renderers")
        // .set<Pool<Renderer>>()
        .set<Nonode>({39,160,25});
    auto drawcalls = ecs.prefab("Drawcalls").set<Nonode>({160,52,25});

// using namespace std;

void newPrefab(flecs::entity e) {

    std::string name = std::string(e.name().c_str())+" "+std::to_string(ecs.count(flecs::IsA, e));
    ecs.entity(name.c_str()).is_a(e);

}

int main() {


// flecs::entity c = 
// ecs.each([](flecs::entity e, Nonode& node){

//     std::cout << e.name() << " hello world\n";

// });
// cout << c.size() << endl;


newPrefab(renderers);
newPrefab(renderers);
newPrefab(renderers);
newPrefab(drawcalls);
newPrefab(renderers);
    




    // new Shadertoy("smartmap"); 

    // TreeviewWindow tree;
    // tree.addPool<DrawCall>("DrawCalls");
    // tree.addPool<Renderer>("Renderers");
    // tree.addPool<Repository>("Repository");

    while(true) window->render([&]() { gui->render(); });

} 

