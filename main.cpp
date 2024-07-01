/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "src/engine.hpp"
#include "effector.hpp"
#include "instance.hpp"
#include "vbo.hpp"


struct WrapperEffector : Effector {

    WrapperEffector() : Effector("ooooo") {

        s.name("wrapper");

        q(1);


    }


    int q_v = 1;
    void q(int q_) {

        q_v = q_;

        auto members = s.members;

        for (auto x : members) delete x;

        for (int i = 0 ; i < q_v; i++) s.add<float>("param_"+std::to_string(i));

    }

//      {

//         source = "void wrapper( ";

//         for (auto x : s.members) source += "float "+x->name()+", ";

//         source.resize(source.size()-2);

//         source +=" ) {";

//         source +="";

//         source +="}";

//     }

};


int main() {


    engine.init();

    Editor<Effector::Definition>([](Node* node, Effector::Definition *def){

        static std::map<Effector::Definition*,TextEditor> codeeditors;

        if (codeeditors.find(def) == codeeditors.end()) {

            codeeditors[def].SetShowWhitespaces(false);
            codeeditors[def].SetReadOnly(false);
            codeeditors[def].SetText(def->source.data());
        }

        auto &codeeditor = codeeditors[def];

        codeeditor.Render("codeeditor");

        if (codeeditor.IsTextChanged()) {

            def->source = codeeditor.GetText().c_str();

            node->bkpupdate(); // do I need bkp here ? is even the fx useful

        }

        draw_definition(&def->s);

    });


    // Effector::Definition wrapper;
    // wrapper.s.name("wrapper");
    // wrapper.args = {{"float","1"},{"float","2"},{"float","3"}};

    // engine.effectors.add

    // engine.open("project.json");

    File fx1("argb.glsl", "//red(0,1,1)\n//green(0,1,1)\n//blue(0,1,1)\n//alpha(0,1,1)\n \nvoid argb(float alpha, float red, float green, float  blue) { \n\n\tcolor = color * vec4 (red, green, blue, 0) * alpha; \n\n}\n\n");

    auto argb_ = engine.effectors->addOwnr<Effector::Definition>(&fx1);
    auto &argb = *argb_->get();

    WrapperEffector w;

    auto wrap_ = engine.effectors->addPtr<Effector>(&w);
    auto &wrap = *wrap_->get();

    logger.cout(Sev::debug);

    auto quad = engine.models->addPtr<File>( &VBO::quad );

    auto q = engine.stack->add(quad);

    q->childrens[0]->add(argb_);

    engine.stack->childrens[0]->update();
    engine.run();

}

// could I remove Effector::definitions and use "Ref Effector::ref"
