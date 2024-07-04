#include "drawcall.hpp"
#include "effector.hpp"
#include "engine.hpp"
#include "utils.hpp"
#include "layer.hpp"


Model* DrawCall::addModel(File* f) {

     auto x = Modelable::addModel(f);

     vbo.add(f, models.size()-1);

     return x;
}

///////// LayerBuilder ////

Layer::ShaderProgramBuilder::ShaderProgramBuilder() : ShaderProgramBuilder(nullptr) { }

Layer::ShaderProgramBuilder::ShaderProgramBuilder(DrawCall* dc) : dc(dc) { build(); }

void Layer::ShaderProgramBuilder::build() {

    stride_count = 0;

    effectors.clear();

    // if(dc) for (auto &model : dc->models) for (auto &effector : model.get()->refs) for (auto x : effector->definitions) ADD_UNIQUE<Effector*>(effectors, x);

    // if(dc) for (auto &effector : dc->effectors) for (auto x : effector->definitions) ADD_UNIQUE<Effector*>(effectors, x);

    ShaderProgram::Builder::build();

}

std::string Layer::ShaderProgramBuilder::print_model(std::string xtra, Model& model) {

    std::string body_fragment;

    // auto name = lower(model.s_->name());

    // if (model.s_->quantity() > 1) name += "["+xtra+"]";

    // body_fragment += "\t// "+name+"\n";

    // body_fragment += "\taspect_ratio = static_ubo.layers"+std::string(Layer::glsl_layers->def()->quantity()>1?"[int(LAYER)]":"")+".dim;\n";
    // body_fragment += "\ttic();\n";

    // for (auto &effector : model.refs) {

    //     std::string arg_str;

    //     if (effector->definitions.size() == 1)  {

    //         for (auto &arg : effector->definitions[0]->s.members) {

    //             arg_str += "dynamic_ubo[curr]."+dc->s.name()+"."+name+"."+effector->s.name()+"."+arg->name()+", ";

    //         }

    //         arg_str.resize(arg_str.size()-2);

    //         body_fragment += "\t"+effector->definitions[0]->s.name()+"("+arg_str+"); // 2\n";

    //     }else if (effector->definitions.size())  {

    //         for (auto &arg : effector->s.members) {

    //             arg_str += "dynamic_ubo[curr]."+dc->s.name()+"."+name+"."+effector->s.name()+"."+arg->name()+", ";

    //         }

    //         arg_str.resize(arg_str.size()-2);

    //         body_fragment += "\t"+effector->s.name()+"("+arg_str+"); d\n";


    //     }
    // }

    return body_fragment;
}
void Layer::ShaderProgramBuilder::common() {

    ShaderProgram::Builder::common();

    header_common += ubo_layout({&engine.dynamic_ubo, &engine.static_ubo});

}


void Layer::ShaderProgramBuilder::frag() {

    header_fragment.clear();

    header_fragment += "uniform sampler2D medias;\n\n";
    header_fragment += "uniform sampler2D render_pass;\n\n";
    header_fragment += "uniform sampler2D uberlayer;\n\n";

    for (int i = 1; i < dc->vbo.vertice->members.size(); i++) {

        auto m = dc->vbo.vertice->members[i];

        header_fragment += "in "+std::string(m->type() == typeid(int)?"flat ":"")+m->type_name()+" "+m->name()+";\n";

    }

    header_fragment += "\n";
    header_fragment += "in flat int ID;\n";

    header_fragment += "vec2 uv = UV;\n";
    header_fragment += "vec4 color = vec4(0);\n";
    header_fragment += "vec2 aspect_ratio = vec2(1);\n\n";

    int model_id = 0;

    // for (auto def : effectors)  header_fragment += def->source+";\n\n";

    header_fragment += "void tic() { COLOR += color; uv = UV; color = vec4(1); }\n";
    header_fragment += "void tac() { COLOR += color; uv = UV; color = vec4(0); }\n\n";
    header_fragment += "int curr = dynamic_ubo[0].eNGINE.alt;\n";
    header_fragment += "int last = abs(curr-1);\n\n";

    header_fragment += comment_line;

    body_fragment.clear();

    if (dc) {

        std::string indice = "";

        if (dc->models.size() == 1) body_fragment += print_model("ID", *dc->models[0].get()) + "\ttac();\n\n";

        else for (auto &model : dc->models) {

            for (int instance = 0; instance < model.get()->s_->quantity(); instance++) body_fragment += print_model(std::to_string(instance), *model.get());

            model_id++;

            body_fragment += "\ttac();\n\n";

        }


    }

    if (dc) {

        for (auto &effector : dc->refs) {

            std::string arg_str;

            // for (auto def : effector->definitions) {


            //     for (auto &arg : def->s.members) {

            //         arg_str += "     dynamic_ubo[cdurr]."+dc->s_->name()+"."+effector->s_->name()+"."+arg->name()+", ";

            //     }

            //     arg_str.resize(arg_str.size()-2);

            //     body_fragment += "\t"+def->s.name()+"("+arg_str+"); // 3\n";

            // }

        }

        // if (dc->effectors.size()) body_fragment += "\ttac();\n\n";

    }

}

void Layer::ShaderProgramBuilder::vert() {

    vbo = &dc->vbo;

    ShaderProgram::Builder::vert();

    for (int i = 1; i < vbo->vertice->members.size(); i++) {

        auto m = vbo->vertice->members[i];

        header_vertex += "out "+std::string(m->type() == typeid(int)?"flat ":"")+m->type_name()+" "+m->name()+";\n";

    }

    header_vertex += "out flat int ID;\n\n";

    body_vertex.clear();

    for (int i = 1; i < dc->vbo.vertice->members.size(); i++) {

        auto m = dc->vbo.vertice->members[i];
        body_vertex += "\t"+m->name()+" = "+m->name()+"_;\n\n";

    }

    body_vertex += "\tID = gl_InstanceID;\n\n";

	body_vertex += "\tvec2 POS = POSITION;\n\n";


}



// DRAWCALLL //////////////////////////////////////

DrawCall::DrawCall(std::string name = "") : Modelable(name.length()?name:"layer"), builder(this) {

    shader.builder = &builder;

    engine.dynamic_ubo.add(s_);

}

void DrawCall::draw() {

    shader.use();

    vbo.draw();

}


void DrawCall::update() {

    static std::filesystem::file_time_type last_modified = std::chrono::file_clock::now();

    static bool has_changed = false;

    for (auto x : models) { // rien a foutre la

        if (x.get()->file->owned) continue;

        auto last_ = std::filesystem::last_write_time(std::filesystem::path(File::loc()) / x.get()->file->path_v);

        if (last_modified  < last_) { last_modified = last_; has_changed = true; }

    } /// ???

    if (has_changed) {

        int i = 0;

        vbo.reset();

        for (auto &x : models) vbo.add_noupload(x.get()->file, i++) ;

        vbo.upload();

        has_changed = false;

    }

    ((ShaderProgram*)&shader)->create();

}
