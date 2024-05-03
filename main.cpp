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

// cant use STRUCT auto rename here ( or could reflect from ? struct auto rename works only when typed()aka.ownd .. expect next name ?)

// naming  _1 _2 ... should be carreful only at node otrherwise de4v should know

// then cant use Node renaming either

// then should have its own renaming but then is one per Mod, per Fx, ... UGLY


struct Fx  {

    static inline std::map<File*, Struct> effects;

    static void init(File* f) {

        effects[f] = Struct(f->name_v.c_str());

        effects[f].add<float>("x");
        effects[f].add<float>("y");

    }

    static Struct& get(File* f) { if (effects.find(f) == effects.end()) init(f); return effects[f]; }

    Struct s;

    Fx(File* f, std::string name ) : s(name) { s.add( &get(f) ); };


};

struct Mod  {

    File* f;

    Struct s;

    std::set<std::shared_ptr<Fx>> fxs;

    Mod(std::string name, File* f) : s(name), f(f) {  };

    Fx* add(File* f) {

        auto fx = fxs.insert(std::make_shared<Fx>(f, s.next_name(f->name()))).first->get();

        s.add(&fx->s);

        return fx;

    }

};


// struct Lay  {

//     Struct s;

//     Lay(std::string name) : s(name) { engine.dynamic_ubo.add(&s); }

//     std::set<std::shared_ptr<Mod>> mods;

//     void add(File* effect) {

//         auto &mod = mods.insert(std::make_shared<Mod>(effect->name_v, effect)).first->get();

//         Struct::add(mod.s);

//         return members.back();

//     }


// };


// struct.add(struct)
int main() {

engine.init();

logger.cout();

auto fe1 = engine.tree->addOwnr<File>("fx.glsl");

auto fm1 = engine.tree->addOwnr<File>("quad.mod");

// auto l1 = engine.tree->addOwnr<Lay>("layer1");
auto m1 = engine.tree->addOwnr<Mod>("quad", fm1->get());



// auto l1 = engine.tree->addOwnr<Lay>();

// NODE<Mod>::oncreate([](Node*node, Mod* mod){ node->name(mod->s.name()); });
// NODE<Mod>::onchange([&](Node*node, Mod* mod){ mod->s.name(node->name()); });

NODE<Struct>::oncreate([](Node*node, Struct* s){ node->name(s->name()); });

NODE<Struct>::onchange([&](Node*node, Struct* s){ s->name(node->name()); });

NODE<Fx>::oncreate([](Node*node, Fx* fx){ NODE<Struct>::oncreate_cb(node, &fx->s); });

NODE<Fx>::onchange([&](Node*node, Fx* fx){ NODE<Struct>::onchange_cb(node, &fx->s); });


NODE<Mod>::onadd<File>([](Node*_this,Node*node){



    auto x =  _this->addPtr<Fx>(_this->is_a<Mod>()->add( node->is_a<File>() ))->node();



    PLOGD <<"-"<<_this->is_a<Mod>()->s.print_recurse() <<"-";
    PLOGD << " fioduoasdf sdfhj fsdhjksdf aghsdfag sdfagj asdhgfhj asdgfjhasd jfgasdhjgfjkh agfhjkdsag jhfgasd hjfgj asgfjhds gfhjsd afd";



    return x; });

// NODE<Lay>::onadd<File>([](Node*_this,Node*node){ return _this->addPtr<Mod>(  _this->is_a<Lay>()->add( node->is_a<File>() ))->node(); });

// NODE<Mod>::oncreate([](Node*node, Mod* mod){ NODE<Struct>::oncreate_cb(node, mod); });
// NODE<Mod>::onchange([&](Node*node, Mod* mod){ NODE<Struct>::onchange_cb(node, mod); });

// auto m1 = l1->add(fm1);
// m1->add(fe1);
// m1->add(fe1);
// m1->add(fe1);
// m1->add(fe1);
// m1->add(fe1);
// m1->add(fe1);
// m1->add(fe1);

// PLOGD << l1->get()->print();


// auto inst2 = l1->add(m1);




PLOGD << "waga";

engine.run();


}
