#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "include/vendor/entt/entt/src/entt/entt.hpp"


// #define NO_GLM 1
// #ifndef NO_GLM
#include "include/vendor/glm/glm/glm.hpp"
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
// #else
// struct vec2 {vec2(float x, float y) : data(x,y) { } float data[2]; float& x = data[0]; float& y = data[1]; };
// struct vec3 { vec3(float x, float y, float z) : data(x,y,z) { } float data[3]; float& x = data[0]; float& y = data[1]; float& z = data[2]; float& r = data[0]; float& g = data[1]; float& b = data[2]; };
// struct vec4 { vec4(float x, float y, float z, float w) : data(x,y,z,w) { } float data[4]; float& x = data[0]; float& y = data[1]; float& z = data[2]; float& w = data[3]; float& r = data[0]; float& g = data[1]; float& b = data[2]; float& a = data[3]; };
// struct mat4 { mat4(float x) : x(x) {} float x; };
// #endif
using uint = uint32_t;
using str = std::string;
#define to_str(x) std::to_string(x)

struct AnyTag {};
#define EZ_TAG(NAME,TYPE,DEFAULT) \
struct NAME : Tag<TYPE> { NAME(TYPE value = DEFAULT) : Tag(value) {} };
template <typename T>
struct Tag : AnyTag { 
    
    T value; 

    static inline std::vector<AnyTag*> pool;

    Tag(T value = T()) : value(value) { pool.push_back(this); /*std::cout << registry. */ }
    operator T& () { return value; }
    void operator=(const T& value) { this->value = value; }
    T operator-(const Tag<T>& other) { return other.value - value; }
    T operator+(const Tag<T>& other) { return other.value + value; }
    bool operator<(const Tag<T>& other) { return other.value < value; }
    bool operator>(const Tag<T>& other) { return other.value < value; }

    const char* type() { return typeid(T).name(); } 
    const char* str() { return std::to_string(value).c_str(); } 

};

entt::registry registry; 

EZ_TAG(UID, float, -1);

EZ_TAG(Label, const char*, "none");

EZ_TAG(RGB, vec3, vec3(1,1,1));

EZ_TAG(RGBA, vec4, vec4(1,1,1,1));

EZ_TAG(Transform, mat4, mat4(1));

EZ_TAG(Parent, entt::entity, entt::null);

EZ_TAG(Child, entt::entity, entt::null);

struct Entity { 

    entt::entity e;

    Entity(entt::entity e) :  e(e) { }

    Entity(std::string label = "") {

        e = registry.create();

        add<UID>(registry.view<UID>().size());

        if (!label.length()) 
            label = "Entity "
            +to_str(registry.view<Label>().size());
            
        registry.emplace<Label>(e, label.c_str());

    }

    template <typename T, typename... Args>
    T& add(Args&& ... args) { return registry.emplace<T>(e, std::forward<Args>(args)...); }

    template <typename T, typename... Args>
    T& get(Args&& ... args) { return registry.get<T>(e, std::forward<Args>(args)...); }

    template <typename T, typename... Args>
    bool has(Args&& ... args) { return registry.all_of<T>(e, std::forward<Args>(args)...); }

    operator const entt::entity& () { return e; }

    // Entity* childOFf(Entity* e) { e->add<Parent>(this->e); return e; }
    Entity* child(std::string label) { Entity* e = new Entity(label); e->add<Parent>(this->e); return e; }
    Entity* child(Entity* e) { e->add<Parent>(this->e); return e; }

    Entity& parent() { return (Entity&)get<Parent>(); }

    static Entity eq(uint i) { auto view = registry.view<UID>(); return Entity(view[i]); }
    static int count() { auto view = registry.view<UID>(); return view.size(); }

};

using namespace entt::literals;

/// 1 - trouble with sortng

/// 2 - trouble with meta & hashed_str entt::meta<UID>().type(51 || "reflected_type"_hs);

/// 3 - checkout observer // registry.on_update<UID>().connect<&test>();

int main(int argc, char *argv[]) {

    for (int i = 0; i < 6; i++) new Entity("Aaa "+std::to_string(i));

    auto p = Entity::eq(2);

    for (int i = 0; i < 3; i++) {

        p.child(new Entity("kinder "+std::to_string(20+i)));
        p.child("kinder "+std::to_string(20+i)); // DOESNT WORK §§§§§ /// /FUCKING HELLL

    }
    
    auto group2 = registry.group<UID>(entt::get<Label>);

    group2.sort<UID>([](UID& b, UID& a) { return  ((a > b) - (a < b)); });

    Entity::eq(5).get<UID>() = 666.999; 

    for (auto entity : group2) {

        Entity e(entity);

        std::cout << "[" << (uint32_t)entity << "]  UID: " << e.get<UID>();
        if (registry.all_of<Label>(entity)) std::cout << ", label: " << e.get<Label>() ;
        if (registry.all_of<Parent>(entity)) std::cout << ", parent: " << e.parent().get<Label>() ;
        if (registry.all_of<RGB>(entity)) std::cout << " and RGB " ;
        if (registry.all_of<RGBA>(entity)) std::cout << " and RGBA " ;
        if (registry.all_of<Transform>(entity)) std::cout << " and Transform " ;
        if (registry.all_of<Child>(entity)) std::cout << " and Child " ;
        std::cout << std::endl; 
        
    }

}


