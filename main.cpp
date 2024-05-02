/*

    SMARTMAP (beta)

                        */

#include "member.hpp"
#include "struct.hpp"
#include "node.hpp"
#include "callbacks.hpp"

#include "engine.hpp"
#include "callbacks.hpp"

#include <memory>
#include <map>


struct Effect  {

    static inline std::map<File*, Struct> effects;

    static void init(File* f) {

        effects[f] = Struct(f->name_v.c_str());

        effects[f].add<float>("x");
        effects[f].add<float>("y");

    }

    static Struct& get(File* f) { if (effects.find(f) == effects.end()) init(f); return effects[f]; }

};

struct Mod : Struct  {

    File* f;

    Mod(std::string name, File* f) : Struct(name), f(f) {  };

    Member* add(File* effect) {

        Struct::add(Effect::get(effect));

        return members.back();

    }


};

// struct Lay : RefList<Mod> { Struct s; Lay() : RefList(&s) { engine.dynamic_ubo.add(s); } };


// struct.add(struct)
int main() {

engine.init();

logger.cout();

auto fe1 = engine.tree->addOwnr<File>("fx.glsl");

auto fm1 = engine.tree->addOwnr<File>("quad.mod");

auto m1 = engine.tree->addOwnr<Mod>("quad", fm1->get());



// auto l1 = engine.tree->addOwnr<Lay>();

// NODE<Mod>::oncreate([](Node*node, Mod* mod){ node->name(mod->s.name()); });
// NODE<Mod>::onchange([&](Node*node, Mod* mod){ mod->s.name(node->name()); });

NODE<Struct>::oncreate([](Node*node, Struct* s){ node->name(s->name()); });

NODE<Struct>::onchange([&](Node*node, Struct* s){ s->name(node->name()); });




NODE<Mod>::onadd<File>([](Node*_this,Node*node){


    return _this->addPtr<Struct>(_this->is_a<Mod>()->add( node->is_a<File>() ))->node();


});


m1->add(fe1);
m1->add(fe1);
// m1->add(fe1);

// NODE<Lay>::onadd<File>([](Node*_this,Node*node){ return _this->addPtr<Mod>(  &_this->is_a<Lay>()->add( node->is_a<File>() ))->node(); });

// auto inst = l1->add(m1);
// auto inst2 = l1->add(m1);




PLOGD << "waga";

engine.run();


}
