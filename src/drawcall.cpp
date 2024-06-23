#include "drawcall.hpp"
#include "effector.hpp"
#include "engine.hpp"
#include "utils.hpp"
#include "layer.hpp"

#include "effector.hpp"
#include "model.hpp"


Effectable::Effectable(std::string name) : s(name) {  }

bool Effectable::removeEffector(Effector* effector) {

    return std::erase_if( effectors, [&](std::shared_ptr<Effector> e) { return e.get() == effector; });

}


Effector* Effectable::addEffector(File* file) {

    if (file->extension != "glsl") { PLOGW << "WARUM :GLSL only BB !!"; return nullptr;}

    auto effector = effectors.emplace_back(std::make_shared<Effector>(file, s.next_name(file->name()))).get();

    s.add(&effector->ref);

    return effector;

}

Model* Modelable::addModel(File* f) {

    auto mod = models.emplace_back(std::make_shared<Model>(f, s.next_name(f->name()))).get();

    // for (auto x : s.getTop()) { // should be once even if its a for

    // //    Instance inst(&x);


    // }

    s.add(&mod->s);

    vbo.add(f, models.size()-1) ;

    return mod;

}

bool Modelable::removeModel(Model* model){ return std::erase_if( models, [&](std::shared_ptr<Model> e) { return e.get() == model; }); }


///////// LayerBuilder ////

Layer::ShaderProgramBuilder::ShaderProgramBuilder() : ShaderProgramBuilder(nullptr) { }

Layer::ShaderProgramBuilder::ShaderProgramBuilder(DrawCall* dc) : dc(dc) { build(); }

void Layer::ShaderProgramBuilder::build() {

    stride_count = 0;

    effectors.clear();

    if(dc) for (auto &model : dc->models) for (auto &effector : model.get()->effectors) ADD_UNIQUE<File*>(effectors, effector->file);

    if(dc) for (auto &effector : dc->effectors) ADD_UNIQUE<File*>(effectors, effector->file);

    ShaderProgram::Builder::build();

}

std::string Layer::ShaderProgramBuilder::prout(std::string xtra, Model& model) {

    std::string body_fragment;

    auto name = lower(model.s.name());

    if (model.s.quantity() > 1) name += "["+xtra+"]";

    body_fragment += "\t// "+name+"\n";

    body_fragment += "\taspect_ratio = static_ubo.layers"+std::string(Layer::glsl_layers->def()->quantity()>1?"[int(LAYER)]":"")+".dim;\n";
    body_fragment += "\ttic();\n";

    // body_fragment += "\t"+camel(name)+" "+name+" = dynubo."+lower(ubl->s.name())+"."+name+(layer.s.quantity() > 1?"[OBJ]":"")+";\n";

    for (auto &effector : model.effectors) {

        std::string arg_str;

        for (auto &arg : Effector::get(effector.get()->file).args) {

            arg_str += "dynamic_ubo[curr]."+dc->s.name()+"."+name+"."+effector->ref.name()+"."+arg.second+", ";
            // arg_str += name+"."+effector->ref.name()+"."+arg.second+", "; // super costly

        }

        arg_str.resize(arg_str.size()-2);

        body_fragment += "\t"+effector->file->name()+"("+arg_str+");\n";
    }

    return body_fragment;
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

    for (auto file : effectors)  header_fragment += Effector::source(file)+";\n\n";

    header_fragment += "void tic() { COLOR += color; uv = UV; color = vec4(1); }\n";
    header_fragment += "void tac() { COLOR += color; uv = UV; color = vec4(0); }\n\n";
    header_fragment += "int curr = dynamic_ubo[0].eNGINE.alt;\n";
    header_fragment += "int last = abs(curr-1);\n\n";
    // header_fragment += "Dynamic_ubo dynubo = dynamic_ubo[curr];\n";
    // header_fragment += "Dynamic_ubo dynubo_last = dynamic_ubo[last];\n\n";

    header_fragment += comment_line;

    body_fragment.clear();

    if (dc) {

        std::string indice = "";

        if (dc->models.size() == 1) body_fragment += prout("ID", *dc->models[0].get()) + "\ttac();\n\n";

        else for (auto &model : dc->models) {

            for (int instance = 0; instance < model.get()->s.quantity(); instance++) body_fragment += prout(std::to_string(instance), *model.get());

            model_id++;

            body_fragment += "\ttac();\n\n";

        }


    }

    if (dc) {

        for (auto &effector : dc->effectors) {

            std::string arg_str;

            for (auto &arg : Effector::get(effector.get()->file).args) {

                arg_str += "dynamic_ubo[curr]."+dc->s.name()+"."+effector->ref.name()+"."+arg.second+", ";

            }

            arg_str.resize(arg_str.size()-2);

            body_fragment += "\t"+effector->file->name()+"("+arg_str+");\n";
        }

        if (dc->effectors.size()) body_fragment += "\ttac();\n\n";

    }

}

void Layer::ShaderProgramBuilder::vert() {

    header_vertex.clear();

    int count = 0;
    for (auto x : dc->vbo.vertice->members) header_vertex += "layout (location = "+std::to_string(count++)+") in "+x->type_name()+" "+x->name()+(count?"_":"")+";\n";
    header_vertex += "\n";

    for (int i = 1; i < dc->vbo.vertice->members.size(); i++) {

        auto m = dc->vbo.vertice->members[i];

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

    engine.dynamic_ubo.add(&s);

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

    shader.create();

}
