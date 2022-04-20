#include <iostream>
#include <string>
#include <vector>
#include "include/vendor/flecs/flecs.h"
#include <ctime>


using uint = uint32_t;
using string = std::string;
using entity = flecs::entity;

flecs::world ecs;


struct Object { };


struct Test { float a,b,c; };


auto inspect(flecs::entity e) {

    e.each([&](flecs::id id) {

        const flecs::MetaTypeSerialized *ser = id.object().get<flecs::MetaTypeSerialized>();
    
        flecs::vector<ecs_meta_type_op_t> ops(ser->ops);
    
        auto id_ptr = e.get(id.object());

        std::cout << "" << id.str().c_str() << "[" << ops.size() << "]: {" << std::endl;
    
        for (auto op : ops) {

            if (!op.name) continue;
            
            std::cout << "  " << op.name << ": ";

             void *ptr = (void*)(((uintptr_t)id_ptr) + op.offset);
             std::cout << *(float*)ptr << ",";
            
            std::cout << std::endl;
           
        }
        
        std::cout << "}" << std::endl;

    });

}

int main(int, char *[]) {

    std::cout << ecs.count<Test>() << std::endl;

    auto e = ecs.entity().set<Test>({1,2,3});
    auto e1 = ecs.entity("E1").set<Test>({3,4,5});
    auto e2 = ecs.entity("E2").is_a(e1);

        auto q = ecs.filter_builder<>().term(flecs::IsA, flecs::Wildcard).build();
        q.each([&](flecs::entity e) {
            std::cout << "e.str().c_str() "<< std::endl; 
        });


    ecs.component<Test>().member<int>("A").member<int>("B").member<int>("C");

    inspect(e);

    // return ecs.app().enable_rest().run();
}
