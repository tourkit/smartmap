#include <iostream>
#include "src/tet.hpp"

//// ImGUI STYLE STRUCT (handwritten reflection ? :( ) TO ENTITY , JSONable to save, an vice versa to load
// runtime component for parsing

    //inspect() un pair !

    // ou rangeer les sytems ?
    // cueliste de systems

    // add quill

template <typename T>
T& recast(const void* ptr, size_t offset) { return *(T*)(void*)(((uintptr_t)ptr) + offset); }





void inspect(flecs::id id, flecs::entity e) {

    flecs::entity f = id.object();

    const flecs::MetaTypeSerialized *ser = f.get<flecs::MetaTypeSerialized>();
    if (!ser) return;
    flecs::vector<ecs_meta_type_op_t> ops(ser->ops);

    std::cout << "  " << id.str().c_str() << " " << f.str();
    std::cout << "[" << ops.size() << "]" << std::endl;

    void* ptr;

    if (id.has_role()) ptr = (void*)e.get(flecs::Wildcard, id.object());
    else ptr = (void*)e.get(id.object());

    for (auto op : ops) { if (!op.name) continue;
        
        std::cout << "  - " << ecs.entity(op.type).name() << " " << op.name << ": ";

        switch (op.kind) {

            case EcsOpBool : std::cout << recast<bool>(ptr, op.offset); break;
            case EcsOpChar : std::cout << recast<char>(ptr, op.offset); break;
            case EcsOpU8 : std::cout << recast<uint8_t>(ptr, op.offset); break;
            case EcsOpU16 : std::cout << recast<uint16_t>(ptr, op.offset); break;
            case EcsOpU32 : std::cout << recast<uint32_t>(ptr, op.offset); break;
            case EcsOpU64 : std::cout << recast<uint64_t>(ptr, op.offset); break;
            case EcsOpI8 : std::cout << recast<int8_t>(ptr, op.offset); break;
            case EcsOpI16 : std::cout << recast<int16_t>(ptr, op.offset); break;
            case EcsOpI32 : std::cout << recast<int32_t>(ptr, op.offset); break;
            case EcsOpI64 : std::cout << recast<int64_t>(ptr, op.offset); break;
            case EcsOpF32 : std::cout << recast<float>(ptr, op.offset); break;
            case EcsOpF64 : std::cout << recast<float>(ptr, op.offset); break;
            case EcsOpString : std::cout << recast<const char*>(ptr, op.offset); break;
            case EcsOpEntity : std::cout << "entity " ;/*<< recast<uint32_t>(ptr, op.offset);*/ break;

            default: break;
        
        }

        std::cout << std::endl;
        
    }

}

struct Tag {};
struct Object {};

struct Foo { int bar; };

int main(int, char *[]) {

    flecs::world ecs;

    ecs.component<Foo>().member<int>("bar");
    ecs.component<EcsIdentifier>().member<char*>("value");

    ecs.entity("jiji").set<Foo>({1});
    ecs.entity().set<Object, Foo>({2});
    ecs.entity().set<Tag, Foo>({3});
    
    auto q = ecs.query_builder<>().term<flecs::Component>().oper(flecs::Not).term(flecs::ChildOf, flecs::Wildcard).oper(flecs::Not);

    q.build().each([](flecs::entity e) {
        
        std::cout << e.str() << std::endl;
        e.each([e](flecs::id id){ inspect(id,e); });
    });

}

    // auto e2 = ecs.entity().set<Tag, Foo>({2});
    // auto e21 = e2.get<Tag,Foo>(); // oui
    // auto e22 = e2.get(ecs.component("Tag"), ecs.component("Foo")); // oui pareils
    // auto e23 = e2.get(ecs.component<Tag>(), ecs.component<Foo>()); // oui pareils
    // // auto e23  = e2.get<Tag>(ecs.component("Foo")); // non