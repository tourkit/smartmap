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

struct Fx  {

    static inline std::map<File*, Struct> effects;

    static void init(File* f) {

        effects[f] = Struct(f->name_v.c_str());

        effects[f].add<float>("x");
        effects[f].add<float>("y");

    }

    static Struct& get(File* f) { if (effects.find(f) == effects.end()) init(f); return effects[f]; }

    Ref ref;

    Fx(File* f, std::string name ) : ref(name) { ref.add( &get(f) ); };


};

struct Mod  {

    File* f;

    Struct s;

    std::set<std::shared_ptr<Fx>> fxs;

    Mod(File* f,std::string name) : s(name), f(f) {  };

    Fx* add(File* f) {

        auto fx = fxs.insert(std::make_shared<Fx>(f, s.next_name(f->name()))).first->get();

        s.add(&fx->ref);

        return fx;

    }

};


struct Lay  {

    Struct s;

    Lay(std::string name) : s(name) { engine.dynamic_ubo.add(&s); }

    std::set<std::shared_ptr<Mod>> mods;

    Mod* add(File* f) {

        auto mod = mods.insert(std::make_shared<Mod>(f, s.next_name(f->name()))).first->get();

        s.add(&mod->s);

        return mod;

    }


};


// struct.add(struct)
int main() {

engine.init();

logger.cout(true);

auto fe1 = engine.tree->addOwnr<File>("fx.glsl");

auto fm1 = engine.tree->addOwnr<File>("quad.mod");

auto l1 = engine.tree->addOwnr<Lay>("layer1");

NODE<Struct>::oncreate([](Node*node, Struct* s){ node->name(s->name()); });
NODE<Struct>::onchange([&](Node*node, Struct* s){ s->name(node->name()); });

NODE<Fx>::oncreate([](Node*node, Fx* fx){ NODE<Struct>::oncreate_cb(node, &fx->ref); });
NODE<Fx>::onchange([&](Node*node, Fx* fx){ NODE<Struct>::onchange_cb(node, &fx->ref); });

NODE<Mod>::oncreate([](Node*node, Mod* mod){ NODE<Struct>::oncreate_cb(node, &mod->s); });
NODE<Mod>::onchange([&](Node*node, Mod* mod){ NODE<Struct>::onchange_cb(node, &mod->s); });

NODE<Mod>::onadd<File>([](Node*_this,Node*node){ return _this->addPtr<Fx>(_this->is_a<Mod>()->add( node->is_a<File>() ))->node();; });

NODE<Lay>::onadd<File>([](Node*_this,Node*node){ return _this->addPtr<Mod>(  _this->is_a<Lay>()->add( node->is_a<File>() ))->node(); });

NODE<Lay>::onchange( [](Node*node, Lay*lay){

PLOGD <<"-"<<lay->s.print_recurse() <<"-";

 });

engine.run();


}
