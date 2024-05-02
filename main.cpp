/*

    SMARTMAP (beta)

                        */

#include "member.hpp"
#include "struct.hpp"
// #include "node.hpp"
// #include "callbacks.hpp"

// #include "engine.hpp"
// #include "callbacks.hpp"

#include <memory>
#include <map>





// //
// struct Ref {

//     File* f;

//     Struct s;

//     Ref(File* f) : f(f), s(f->name()) {  };

// };

// template <typename T>
// struct RefList {

//     Struct *owner;

//     std::vector<std::shared_ptr <Ref> > list;

//     RefList(Struct *s = nullptr) : owner(s) {  };

//     virtual T& add(File *f) {

//         auto x = std::make_shared<T>(f);

//         int count = 0;
//         for (auto x : list) if (f == x.get()->f) count++;

//         list.emplace_back(x);

//         auto y = list.back();

//         if (count) y.get()->s.name( y.get()->s.name() + "_" + std::to_string(count) );

//         if (owner) owner->add(y.get()->s);

//         return *x.get();

//     }

// };

// struct Effect : Ref {

//     static inline std::map<File*, Struct> effects;

//     static void init(File* f) {

//         effects[f] = Struct(f->name_v.c_str());

//         effects[f].add<float>("x");
//         effects[f].add<float>("y");

//     }

//     static Struct& get(File* f) { if (effects.find(f) == effects.end()) init(f); return effects[f]; }

//     Effect(File* f) : Ref(f) { s.add(Effect::get(f)); };

// };

// struct Mod : Ref, RefList<Effect> { Mod(File* f) : Ref(f), RefList(&(Ref::s)) { }; };

// struct Lay : RefList<Mod> { Struct s; Lay() : RefList(&s) { engine.dynamic_ubo.add(s); } };


// struct.add(struct)
int main() {

// engine.init();

logger.cout();

// auto e1 = engine.tree->addOwnr<File>("fx.glsl");

// auto m1 = engine.tree->addOwnr<File>("quad.mod");

// auto l1 = engine.tree->addOwnr<Lay>();

// NODE<Mod>::oncreate([](Node*node, Mod* mod){ node->name(mod->s.name()); });

// NODE<Mod>::onchange([&](Node*node, Mod* mod){ mod->s.name(node->name()); });

// NODE<Lay>::onadd<File>([](Node*_this,Node*node){ return _this->addPtr<Mod>(  &_this->is_a<Lay>()->add( node->is_a<File>() ))->node(); });

// auto inst = l1->add(m1);
// auto inst2 = l1->add(m1);

// PLOGD << "waga";



//current s.add(s) is shit it shjouldnt work like trhat

Struct aaa("AAA");
aaa.add<float> ("x");
aaa.add<float> ("y");


Struct bbb("BBB");


// bbb.add(aaa);

PLOGD << bbb.print();

// engine.run();


}
