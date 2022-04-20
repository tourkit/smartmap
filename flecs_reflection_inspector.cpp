#include <iostream>
#include "flecs/flecs.h"

template <typename T>
T& recast(const void* ptr, size_t offset) { return *(T*)(void*)(((uintptr_t)ptr) + offset); }

struct Foo { float b; int a; std::string r; };

int main(int, char *[]) {

    flecs::world ecs;
    
    ecs.component<Foo>().member<float>("B").member<int>("A").member(flecs::String, "R");

    auto foo = ecs.entity("foo").set<Foo>({1.0f,2,"3"});

    foo.each([&](flecs::id id) {

        if (id.has_role()) return;

        const flecs::MetaTypeSerialized *ser = id.object().get<flecs::MetaTypeSerialized>();
    
        flecs::vector<ecs_meta_type_op_t> ops(ser->ops);
    
        auto ptr = foo.get(id.object());

        std::cout << "" << id.str().c_str() << " " << foo.name() << "[" << ops.size() << "]" << std::endl;
    
        for (auto op : ops) {

            if (!op.name) continue;
            
            std::cout << "  " << ecs.entity(op.type).name() << " " << op.name << ": ";
            
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
                case EcsOpEntity : std::cout << "entity " << recast<uint32_t>(ptr, op.offset); break;

                default: break;
            
            }

            std::cout << std::endl;
           
        }
        
    });

    // Prints : 
        // Foo foo[8]
        // f32 B: 1
        // i32 A: 2
        // string R: 3
}
