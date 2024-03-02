#include "test.hpp"
#include "log.hpp"
#include "engine.hpp"
#include "node.hpp"
#include "drawcall.hpp"
#include "component.hpp"
#include "effector.hpp"
#include "model.hpp"

#include <vector>

Test::Test() {

    // atlas is fucked


    // dc->is_a<Layer>()->shader.sendUniform("texture0", (int)tex->id);
    // tex->bind();
    // auto tex = new Texture("assets/medias/boy.jpg");
    // ndi->onrun([](Node* n) { 

    //     auto sender = n->is_a<NDI::Sender>();
    //     sender->send()

    //  });

    // auto dc = tree->addOwnr<DrawCall>();;
    // this->dc = dc->node();
    // dc->name = "engine";
    // dc->addPtr(models->childrens[0]);

    auto l = engine.stack->child("layer1")->is_a<Layer>();

    std::string frag_shader;

    frag_shader += "void main() {\n\n";
    frag_shader += "\tCOLOR = vec4(0);\n\n";

    int model_id = 0;
    for (auto &model : l->vbo.models) {

        for (int instance = 0; instance < model->obj->reserved; instance++) {

            auto name = model->file->name+std::to_string(model_id)+"["+std::to_string(instance)+"]";

            frag_shader += "\t// "+name+"\n"; // would love this to be a node name instead
            frag_shader += "\tuv = UV;\n";
            frag_shader += "\tcolor = vec4(1);\n";
            
            if (true) for (auto effector : model->effectors) { 
                
                std::string arg_str;

                auto comp = Component::id(effector->file->name.c_str()); 

                if (comp->members.size()<2) arg_str += name+"."+effector->file->name;

                else for (auto &m: comp->members) {
                    
                    arg_str += name+"."+effector->file->name+"."+m.name;

                    if (&m != &comp->members.back()) arg_str += ", ";
                    
                }

                frag_shader += "\t"+effector->file->name+"("+arg_str+");\n";
            }

            frag_shader += "\tCOLOR += color;\n\n";

        }

        frag_shader += "\n\n";

        model_id++;

    }
    
    frag_shader += "} ";


    PLOGW << frag_shader ;

}