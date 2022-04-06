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

struct Node { float BGcolor[3] = {255,255,255}; };

struct TEST_GUI : GUIRenderer {
    
    TEST_GUI() {

    }

    static void iterate_thru(flecs::entity e, Node& node) {

            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(node.BGcolor[0],node.BGcolor[1],node.BGcolor[2],255));

            ImGui::Text(e.name().c_str());

            ImGui::PopStyleColor();

    }

    void draw() override { 

        ImGui::ShowDemoWindow();

        ImGui::Begin("Tree");

        ecs.each(&iterate_thru);

        ImGui::End();

     }

} test_gui;

flecs::entity newPrefab(flecs::entity e) {

    std::string name = std::string(e.name().c_str());
    int count = ecs.count(flecs::IsA, e);
    name += " "+std::to_string(count+1); //cant if, prefab allready named
    return ecs.entity(name.c_str()).is_a(e);

}


auto renderers = ecs.prefab("Renderer")
    .set<Node>({39,160,25});

auto drawcalls = ecs.prefab("Drawcall")
    .set<Node>({160,52,25});


struct Vertice { float coord[2]; };
struct Indice { unsigned int id[3]; };

struct Mesh {

    std::vector<float> vertices = {{22,33}};
    std::vector<Indice> indices;

};

struct VAO {

    // vector<Mesh*> meshs;

    unsigned int id, vertices, indices;

    ~VAO() { glDeleteBuffers(1, &vertices); glDeleteBuffers(1, &indices); glDeleteVertexArrays(1, &id);  }

    VAO() { glGenBuffers(1, &vertices); glGenBuffers(1, &indices); glGenVertexArrays(1, &id); }

    void send() {

        glBindVertexArray(id);

        // glBindBuffer(GL_ARRAY_BUFFER, vertices);
        // glBufferData(GL_ARRAY_BUFFER,  mesh.vertices.size()*sizeof(Vertice) , &mesh.vertices[0], GL_DYNAMIC_DRAW );

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size()*sizeof(Indice) , &mesh.indices[0], GL_DYNAMIC_DRAW );

        // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (GLvoid *) 0);
        // glEnableVertexAttribArray(0); 

    }
    
};

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



using namespace std;
int main() {

    auto dc1 = ecs.entity("dc1")
        .is_a<Drawcall>()
        .set<VAO>({});

    ecs.entity().set<Vertice>({0,0}).child_of(dc1);
    ecs.entity().set<Vertice>({0,1}).child_of(dc1);
    ecs.entity().set<Vertice>({1,0}).child_of(dc1);
    ecs.entity().set<Vertice>({1,1}).child_of(dc1);

    newPrefab(renderers);
    newPrefab(renderers);
    newPrefab(drawcalls);
    newPrefab(renderers);


    // Iterathe thru pair
ecs.filter_builder<Vertice>().term(flecs::ChildOf, dc1).build().each([](flecs::entity e, Vertice& v) { cout << e.str() << endl; });

    // ecs.each(ecs.pair(flecs::ChildOf, dc1), [](flecs::entity e) { cout << e.str() << " - "  << e.get<Vertice>().coord[0] << endl; });
    // ecs.each([](flecs::entity e, Vertice& vertice) { cout << e.str() << " - " << vertice.coord[0] << endl; });
    // ecs.each(ecs.is_a(flecs::ChildOf), [](flecs::entity e) { cout << e.str() << endl; });

    // Iterathe thru children
    // dc1.children([](flecs::entity e) { ... });



    cout << ecs.count(flecs::ChildOf, dc1);
    // ecs.each([](flecs::entity e, flecs::pair<flecs:Parent, World> p)

    while(true) window->render([&]() { 
        
        ecs.each([](flecs::entity e, VAO& dc) {



        });
        
        gui->render();
        
         });

} 

