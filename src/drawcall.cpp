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

Layer::ShaderProgramBuilder::ShaderProgramBuilder(DrawCall* dc) : dc(dc) {

    vbo = &dc->vbo;
    shader = &dc->shader;

    ubos.push_back(&engine.dynamic_ubo);
    ubos.push_back(&engine.static_ubo);

    for (auto ref : dc->effector_refs) ref.get()->effector->setup(this); // does it do shit ?

    build();

}

std::string Layer::ShaderProgramBuilder::print_arg(Struct* s, std::string prefix) {  // should be Instance

    std::string out;

    for (auto &arg : s->ref()?s->ref()->members:s->members)
        out += prefix+s->name()+"."+arg->name()+", "; // for this nbeed inst

    if (out.length()) out.resize(out.size()-2);

    out = "\t"+s->name()+"("+out+"); // 3\n";

    return out;


}


void Layer::ShaderProgramBuilder::build() {

    Builder::build();

    // FRAGMENT ////////////////

    header_fragment.clear();

    header_fragment += "\n";
    header_fragment += "in flat int ID;\n";

    header_fragment += "vec2 uv = UV;\n";
    header_fragment += "vec4 color = vec4(0);\n";
    header_fragment += "vec2 aspect_ratio = vec2(1);\n\n";

    header_fragment += "void tic() { COLOR += color; uv = UV; color = vec4(1); }\n";
    header_fragment += "void tac() { COLOR += color; uv = UV; color = vec4(0); }\n\n";
    header_fragment += "int curr = dynamic_ubo[0].eNGINE.alt;\n";
    header_fragment += "int last = abs(curr-1);\n\n";

    effectors_fragment.clear();
    body_fragment.clear();
    body_fragment += "\tCOLOR = vec4(0);\n\n";

    if (dc) {

        int model_id = 0;

        for (auto x : dc->effector_refs) x.get()->effector->setup(this);
        for (auto &model : dc->models) for (auto x : model->effector_refs) x.get()->effector->setup(this);
        
        std::string indice = "";

        if (dc->models.size() == 1)
            body_fragment += print_model("ID", *dc->models[0].get()) + "\ttac();\n\n";

        else for (auto &model : dc->models) {

            for (int instance = 0; instance < model.get()->s_->quantity(); instance++)
                body_fragment += print_model(std::to_string(instance), *model.get());

            model_id++;

            body_fragment += "\ttac();\n\n";

        }


    }

    if (dc) {

        // for (auto &ref : dc->effector_refs) body_fragment += print_arg(&ref.get()->s);

        if (dc->effector_refs.size()) body_fragment += "\ttac();\n\n";

    }

    // VERTEX ////////////////

    header_vertex.clear();

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
	body_vertex += "\tgl_Position = vec4(POS, 0, 1);\n\n";


}

std::string Layer::ShaderProgramBuilder::print_model(std::string xtra, Model& model) {

    std::string body_fragment;

    auto name = lower(model.s_->name());

    if (model.s_->quantity() > 1) name += "["+xtra+"]";

    body_fragment += "\t// "+name+"\n";

    body_fragment += "\taspect_ratio = static_ubo.layers"+std::string(Layer::glsl_layers->def()->quantity()>1?"[int(LAYER)]":"")+".dim;\n";
    body_fragment += "\ttic();\n";

    PLOGE << "===============================";

    
    
    model.instance->def()->each([&](Instance& isnt){

        // body_fragment += print_arg(isnt, "dynamic_ubo[curr]."+dc->s_->name()+".");
        // PLOGE << "dynamic_ubo[curr]."+dc->s_->name()+"."+isnt.stl_name();


    });
// 
    for (auto &ref : model.effector_refs) {
    
 
   body_fragment +=  print_arg( &ref.get()->s , "dynamic_ubo[curr]."+dc->s_->name()+"."+model.s_->name()+".");
    
    }

    return body_fragment;
}




// DRAWCALLL //////////////////////////////////////

DrawCall::DrawCall(std::string name) : Modelable(name.length()?name:"layer"), builder(this) {

    shader.builder(&builder);

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

    builder.build();
    ((ShaderProgram*)&shader)->create();

}
