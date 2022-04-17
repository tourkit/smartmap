#include <iostream>
#include <string>
#include <vector>
#include "include/vendor/flecs/flecs.h"
#include <ctime>


using uint = uint32_t;
using string = std::string;
using entity = flecs::entity;

int float_sort(float a, float b) { return (a > b) - (a < b); }


flecs::world ecs;

struct Position { float x, y; };

struct UID { /* float id; // no need */ };

// TODO
// 1 - get pair value parametricly

// 2 - increment UID


// filter query by pair value
// access pair value ?

void inspector(flecs::entity e, std::string offset = "") {

    e.each([e, offset](flecs::id id){

        std::string output = "["+std::to_string((int) id) + "]    ";
            output += offset;
        
        // output += id.ty
        if (id.has_role()) {
        
            output += id.role_str();
            output += ": (";
            
            if (id.is_pair()) {
        
                output += id.first().name();
                output+= ":";
                output += "value";
                output += ", ";
                output += id.second().str();

                const void* ptr = e.get(id);
                // static_cast<id.first().type()>(ptr)->value;

            } else {
        
                output += id.first().name();
            }
            
            output += ")";
            
        }else{

            output += "TAG: ";
            output += id.str();
        }
        
        std::cout <<  output  << std::endl;// << "----------------------" << std::endl ; 
    });
    
}

void explorer(flecs::entity parent, std::string offset = "") {

    auto q = ecs.query_builder<>().
        term(flecs::ChildOf, parent).
        build();
 
    std::cout << "[" << (int)parent << "]";
    std::cout << offset;
    std::cout << parent.name();
    // std::cout <<"(" << parent.get<Node>()->id << ")" << std::endl;

    if (parent.has<UID>()) std::cout << " - UID: " ;//<< parent.get<UID>()->id;

    std::cout << std::endl;
    inspector(parent, offset); 
    std::cout << "----------------------"<< std::endl ;
    q.each([offset](flecs::entity e){ explorer(e, "    "+offset); });
    
    std::cout << std::endl;

}


int ccount = 1;
flecs::entity& create(std::string label = "") {
    
    if (!label.length()) label = "Entity ";// + std::to_string(ecs.count<UID>());
    return ecs.entity(label.c_str()).add<UID>((float)ccount++); 

}

int main(int, char *[]) {

    auto z1 = create("Zeubi1"); 
    auto z2 = create("Zeubi2");
    z2.child_of(z1);
    
    // std::cout << std::endl ;
    // e5.set([](UID& uid) { uid.value = 10; });
    
    // refleciton test
    // ecs.component<Position>().member<float>("x").member<float>("y");
    // ecs.component<UID>().member<float>("id");

    // OnSet event test
    // ecs.trigger<Node>("OnSetPosition").event(flecs::OnSet).each([](flecs::entity e, Node& n) { std::cout << e.name()  << " has been set" << std::endl; } );

    // quick populating test
    // float pos[2] = {1,2}; ecs.each([&](flecs::entity e, Position& p){ p = {pos[0]++,pos[1]++}; }); //usefull

    std::cout << "----------------------" << std::endl;

    auto q = ecs.query_builder<>().
        term(flecs::ChildOf, flecs::Wildcard).oper(flecs::Not).
        term<flecs::Component>().oper(flecs::Not).
        term(flecs::Module).oper(flecs::Not).
        // order_by(id_sort). // WAS WORKING COOL // int id_sort(flecs::entity_t e1, const Node *p1, flecs::entity_t e2,const Node *p2) { return float_sort(p1->id, p2->id); }
        build();

    q.each([](flecs::entity e) { explorer(e); });     
            
}




