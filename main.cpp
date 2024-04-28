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


struct Effect {

    File* f;

    Struct s;

};

struct Mod {  File* f; Mod(File* f) : f(f) {}; };

struct EffectInst {

    Effect* e;

    Struct s;

    EffectInst(Effect* e) : e(e), s("efinst") { s.add(e->s); }

};

struct ModInst {

    Mod* m;

    Struct s;

    std::vector<std::shared_ptr <EffectInst> > efs;

    ModInst(Mod* m, const char* name = "") : m(m), s(name) {}

    void add(Effect *e) { efs.emplace_back(std::make_shared<EffectInst>(e)); s.add(efs.back()->s); }

    void remove(EffectInst* i) { for (auto it = efs.begin(); it != efs.end();) if (it->get() == i) { it = efs.erase(it); break; } else ++it; } // le pb c pas kjekriv inline, el pb c la syntaxx du cpp pour suppr ds un vec

};

struct Lay {

    Struct s;

    std::vector<std::shared_ptr <ModInst> > mods;

    ModInst* add(Mod *m) {

        mods.emplace_back(std::make_shared<ModInst>(m));

        auto x =  mods.back().get();

        x->s.name(m->f->name()+"_"+std::to_string( mods.size() ));

        s.add(mods.back()->s);

        return x;

    }

    void remove(ModInst* i) { for (auto it = mods.begin(); it != mods.end();) if (it->get() == i) { it = mods.erase(it); break; } else ++it; } // le pb c pas kjekriv inline, el pb c la syntaxx du cpp pour suppr ds un vec

};

int main() {


File f;
f.name_v = "f1";



engine.init();

logger.cout();

auto &e1 = *engine.tree->addOwnr<Effect>()->get();
e1.s.name("ef1");
e1.s.add<float>("x");
e1.s.add<float>("y");

auto *m1_ = engine.tree->addOwnr<Mod>(&f);
auto &m1 = *m1_->get();

auto *l1_ = engine.tree->addOwnr<Lay>();
auto &l1 = *l1_->get();



NODE<ModInst>::oncreate([](Node*node, ModInst* inst){

    node->name(inst->s.name());

});

NODE<Lay>::onadd<Mod>([](Node*_this,Node*node){

    _this->is_a<Lay>()->add( node->is_a<Mod>() );

    return _this->addPtr<ModInst> (  _this->is_a<Lay>()->mods.back().get() )->node();

 });


l1_->add(m1_);

l1.mods[0]->add(&e1);

PLOGD << l1.s.print(4);

l1.mods[0].get()->remove(l1.mods[0].get()->efs[0].get());

PLOGD << l1.s.print(4);
PLOGD << "waga";


NODE<ModInst>::onchange([&](Node*node, ModInst* inst){

    inst->s.name() = node->name();
    PLOGD << l1.s.print(4);
});


engine.run();


}
