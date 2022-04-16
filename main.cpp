#include <iostream>
#include <string>
#include <vector>
#include "include/vendor/flecs/flecs.h"
#include <ctime>


flecs::world ecs;

struct Position { float x, y; };

struct Node { 

float r,g,b; 
float id; 

};

void inspector(flecs::entity e) {

    std::cout << "----------------------" << std::endl;

    e.each([e](flecs::id id){

        std::string output = std::to_string((int) id) + " - ";
        // output += id.ty
        if (id.has_role()) {
        
            output += " [";
            output += id.role_str();
            output += ": ";

            if (id.is_pair()) {
        
                output += id.first().name();
                output+= "(";
                // output+=id.first()-> //.type().str();
                output += "), ";
                output += id.second().str();

                const void* ptr = e.get(id);
                // static_cast<id.first().type()>(ptr)->value;

            } else {
        
                output += id.first().name();
            }
            
            output += "]";
            
        }else{
        
            output += id.str();
        }
        
        std::cout <<  output  << std::endl << "----------------------" << std::endl ; 
    });
    
}

void explorer(flecs::entity parent, std::string offset = "+ ") {

    auto q = ecs.query_builder<>().
        term(flecs::ChildOf, parent).
        build();
 
    std::cout << "[" << (int)parent << "] ";
    std::cout << offset;
    std::cout <<  parent.name() << "(" << parent.get<Node>()->id << ")" << std::endl;
        
    q.each([offset](flecs::entity e){ explorer(e, "  "+offset); });

}

int float_sort(float a, float b) { return (a > b) - (a < b); }
int id_sort(flecs::entity_t e1, const Node *p1, flecs::entity_t e2,const Node *p2) { return float_sort(p1->id, p2->id); }

int main(int, char *[]) {

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    auto test = ecs.prefab().
        set_override<Position>({}).
        set_override<Node>({1.0f,1.0f,1.0f})
        ;

    auto ccc = ecs.entity("ooo").add(flecs::IsA,test);
    ecs.entity("ccc1").add(flecs::IsA, test).add(flecs::ChildOf, ccc);
    auto bbb = ecs.entity("bbb").add(flecs::IsA,test);
    ecs.entity("ccc2").add(flecs::IsA, test).add(flecs::ChildOf, ccc);
    ecs.entity("aaa").add(flecs::IsA,test);

    float pos[2] = {1,2};
    ecs.each([&](flecs::entity e, Position& p){ p = {pos[0]++,pos[1]++}; });

    std::cout << "----------------------" << std::endl;

    auto q = ecs.query_builder<Node>().
        term(flecs::ChildOf, flecs::Wildcard).oper(flecs::Not).
        term<flecs::Component>().oper(flecs::Not).
        term(flecs::Module).oper(flecs::Not).
        order_by(id_sort).
        build();



 ecs.trigger<Node>("OnSetPosition")
        
        .event(flecs::OnSet)
        .each([](flecs::entity e, Node& n) { std::cout << e.name()  << " has been set" << std::endl; } );

        std::cout << "aaaaaaaa" << std::endl;
        // bbb.get_mut<Node>()->id = -2.0f;
        bbb.modified<Node>();
        // bbb.set<Node>({1,1,1});
        
        std::cout << "bbbbbbbb" << std::endl;



    q.each([](flecs::entity e, Node& n) { explorer(e); });


        
    // auto query = ecs.filter_builder<>().term(flecs::ChildOf, flecs::IsA).oper(flecs::Not).build();
    // query.each([](flecs::iter& it, size_t i) { std::cout << it.entity(i).str() << std::endl; });
}




