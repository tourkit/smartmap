#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "include/vendor/entt/entt/src/entt/entt.hpp"


#define NO_GLM 1
#ifndef NO_GLM
#include "include/vendor/glm/glm/glm.hpp"
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
#else
struct vec2 {vec2(float x, float y) : data(x,y) { } float data[2]; float& x = data[0]; float& y = data[1]; };
struct vec3 { vec3(float x, float y, float z) : data(x,y,z) { } float data[3]; float& x = data[0]; float& y = data[1]; float& z = data[2]; float& r = data[0]; float& g = data[1]; float& b = data[2]; };
struct vec4 { vec4(float x, float y, float z, float w) : data(x,y,z,w) { } float data[4]; float& x = data[0]; float& y = data[1]; float& z = data[2]; float& w = data[3]; float& r = data[0]; float& g = data[1]; float& b = data[2]; float& a = data[3]; };
#endif
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

EZ_TAG(UID, uint32_t, registry.view<UID>().size());

EZ_TAG(Label, std::string, "Entity "+to_str((int)registry.view<Label>().size()));

EZ_TAG(RGB, vec3, vec3(1,1,1));

EZ_TAG(RGBA, vec4, vec4(1,1,1,1));

EZ_TAG(Position, vec3, vec3(1,1,1));

EZ_TAG(Parent, entt::entity, entt::null);

EZ_TAG(Child, entt::entity, entt::null);

// struct MySquare {


// };


// example template with args...


struct Entity { 

    entt::entity e;

    Entity(std::string label) {

        e = registry.create();

        add<UID>();
        add<Label>(label.c_str());

    }

    template <typename T, typename... Args>
    T& add(Args&& ... args) { return registry.emplace<T>(e, std::forward<Args>(args)...); }

};

int main(int argc, char *argv[]) {

    // registry.on_update<UID>().connect<&test>(); // see observer as well

    for (int i = 0; i < 4; i++) {

        Entity e("Aaa "+std::to_string(i));
    }

    auto view = registry.view<UID>() | registry.view<Label>();
    auto group = registry.group<>(entt::get<UID,Label>);

    registry.get<UID>(group[0]) = 22;

    // entt::meta<UID>().type("ooo"_hs);

    group.sort<UID>([](UID& a, UID& b) { return (a.value > b.value); });

    for (auto entity : group) {
        std::cout << "entity " << (uint32_t)entity << " has UID " << registry.get<UID>(entity) << std::endl;
    }
    // auto group = registry.group<UID>();

}


