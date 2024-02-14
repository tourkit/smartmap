#include "test.hpp"
#include "log.hpp"

#include "node.hpp"

#include <cstring>
#include <typeinfo>
#include <typeindex>
#include <functional>
#include <vector>
#include <map>

#include "buffer.hpp"
#include "engine.hpp"
#include "struct.hpp"
#include "entry.hpp"
#include "vbo.hpp"

// struct Foo { virtual Foo() { PLOGD << "foo"; } };
// struct Bar : Foo { Bar() { PLOGD << "bar"; Foo(); } };



#include <string>
#include <vector>


struct TAnyNode;

struct TempNode {

    std::string name;

    TempNode(std::string name = "nullNode") :name(name) {}

    TempNode* parent_node = nullptr;
 
    std::vector<TempNode*> childrens;

    TAnyNode* add(TempNode *node)  { 
        
        node->parent_node = this;

        childrens.push_back(node);

        return (TAnyNode*)node;

    }

    virtual ~TempNode() {

        for (auto c : childrens) delete c;

        PLOGD <<"dtroy "<<name;

    }

};

template <typename T>
struct TTypedNode : TempNode { 

    T* ptr; 

    bool owned = false;
    
    TTypedNode(void* ptr = nullptr) : ptr((T*)ptr) {}

    template <typename U>
    TTypedNode<U>* addPtr(U* ptr, bool owned = false) { 

        auto new_U = new TTypedNode<U>(ptr);

        new_U->owned = owned;

        TempNode::add(new_U);

        return new_U;

    }

    template <typename U, typename... Args>
    TTypedNode<U>* addOwnr(Args&&... args) {

        auto ptr = new U(std::forward<Args>(args)...);
    
        auto* NEW_U = addPtr(ptr,true);

        if (!NEW_U) { delete ptr; return nullptr; }

        return NEW_U;

    }

    ~TTypedNode() override { 
        
        if (owned) { 
            delete ptr; 
    } 
    }
   
};

template <typename T>
struct TOwnr : TTypedNode<T> {  

    template <typename... Args>
    TOwnr(Args&&... args) { addOwnr(std::forward<Args>(args)...); }

};

template <typename T>
struct TPtr : TTypedNode<T> {  

    template <typename... Args>
    TPtr(T *ptr) { addPtr(ptr,true); }

};

struct Any {};
struct TAnyNode : TTypedNode<Any> {};

struct Foo { Foo() { PLOGD << "foo"; } ~Foo() { PLOGD << "~ foo"; } };
struct Bar {  ~Bar() { PLOGD << "~ bar"; }  Bar(int x) { PLOGD << "bar"; }};

Test::Test(){

        Foo foo;
    {    
    
        TempNode a("aaa");

        auto b = a.add(new TempNode("b"));

        b->addPtr<Foo>(&foo);

        auto z  = b->addOwnr<Bar>(6);
    
    }


    // auto obj = engine.dynamic_ubo->buffer.addObj(new Struct("MICHEL"));

    // obj->s->addComp({"Position"});

    // obj->push();

    // Buffer bkp = *obj->buffer;

    // obj->update2(bkp); // get why this crashess

    // PLOGD << "tete";


}