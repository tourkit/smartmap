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

// TODO
// 1 - get pair value parametricly

// 2 - increment UID


// filter query by pair value
// access pair value ?

//EnTT check for flay value // en cours par skypjack sur discord

//flecs : a can Inherit this type flag/pair
// how oneof works ?

struct UID { float id; operator const float&() const { return id; } };
struct Object { };
struct ids { float idd; operator const float&() const { return idd; }  };

std::vector<flecs::id> inspector(flecs::entity e, std::string offset = "") {

    std::vector<flecs::id> ids;

    e.each([&](flecs::id id){

        std::string output = "["+std::to_string((int) id) + "] ";

         if  ((uint)id < 10) output += " ";
         if  ((uint)id < 100) output += " ";
         output += offset;
         output += "+ ";
         output += std::to_string(ids.size());
         output += "-";
         
        ids.push_back(id);
        
        if (id.is_pair()) {
    
            output += "(";
            output += id.first().name();
            output += ", ";
            output += id.second().str();
            output += ")";
            
        }else{

            output += id.str();
            output += " - ";
            output += id.relation().name();

        }
        

        std::cout <<  output  << std::endl;
    });

    return ids;
    
}

void explorer(flecs::entity parent, std::string offset = "") {

    auto q = ecs.query_builder<>().
        term(flecs::ChildOf, parent).
        build();
 
    std::cout << "[" << (int)parent << "] ";
    std::cout << offset;
    std::cout << parent.name();

    std::cout << std::endl;
    inspector(parent, offset); 
    std::cout << "----------------------"<< std::endl ;
    q.each([offset](flecs::entity e){ explorer(e, "    "+offset); });
    
    std::cout << std::endl;

}

int ccount = 1;
flecs::entity& create(std::string label = "") {
    
    if (!label.length()) label = "Entity ";// + std::to_string(ecs.count<UID>());
    return ecs.entity().set_name(label.c_str()).set<UID>({(float)ccount++}); 

}

int main(int, char *[]) {

    auto e = create("kikoo");
    create("kikoo2 ");

    // e.member(0);
    // auto e = ecs.entity("lolo");
        // .add<Object,UID>()

    std::cout << e.get<UID>()->id << std::endl;
    
    ecs.component<UID>().member<float>("id");
    
    auto view = inspector(ecs.component<UID>());
    
    view[3];
    
    auto q2 = ecs.query_builder<>().
        term(flecs::ChildOf, flecs::Wildcard).oper(flecs::Not).
        term<flecs::Component>().oper(flecs::Not).
        term(flecs::Module).oper(flecs::Not).
        // order_by(id_sort). // WAS WORKING COOL // int id_sort(flecs::entity_t e1, const Node *p1, flecs::entity_t e2,const Node *p2) { return float_sort(p1->id, p2->id); }
        build();

    std::cout << "----------------------" << std::endl;
    q2.each([](flecs::entity e) { explorer(e); });  

// :    ecs.component(flecs::Wildcard).each([](flecs::id id) { std::cout << ecs.get((entity)id).str() << std::endl; });
    
    // std::cout << ecs.to_expr(&e).c_str() << std::endl;
}

// how pair waorks ?
// how  PAIR: (Identifier, Name) is linked to data? how to get that data ? parametricaly !

// why name is pair and not component ?