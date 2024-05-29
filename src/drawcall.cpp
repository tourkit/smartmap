#include "drawcall.hpp"
#include "effector.hpp"
#include "file.hpp"
#include "log.hpp"
#include "ubo.hpp"
#include "engine.hpp"
#include "atlas.hpp"
#include "texture.hpp"
#include "model.hpp"

#include <unordered_set>
#include <set>


///////// Layer ////

Layer::Layer(uint16_t width, uint16_t height, std::string name)

    : fb((width?width:engine.window.width), (height?height:engine.window.height)), DrawCall(name) {

    feedback = new Texture(fb.width,fb.height,2,1, GL_RGB8);

}

void Layer::draw() {

    if (feedback) { feedback->bind(); }

    fb.bind();
    // engine.render_passes[0].bind();


    DrawCall::draw();

    if (feedback) { return feedback->read(fb.texture); }

}




///////// LayerBuilder ////

Layer::ShaderProgramBuilder::ShaderProgramBuilder() : ShaderProgramBuilder(nullptr) { }

Layer::ShaderProgramBuilder::ShaderProgramBuilder(DrawCall* dc) : dc(dc) { build(); }

void Layer::ShaderProgramBuilder::build() {

    stride_count = 0;

    effectors.clear();

    for (auto dc_ : engine.stack->childrens) {

        auto dc = dc_->is_a<Layer>();

        for (auto &model : dc->models) for (auto &effector : model.get()->effectors) effectors.insert(effector->file);

        for (auto &effector : dc->effectors) effectors.insert(effector->file);

    }

    ShaderProgram::Builder::build();

}


void Layer::ShaderProgramBuilder::frag() {

    header_fragment.clear();

    header_fragment += "uniform sampler2D medias;\n\n";
    header_fragment += "uniform sampler2D render_pass;\n\n";

    header_fragment += "in vec2 UV;\n\n";
    header_fragment += "out vec4 COLOR;\n\n";
    header_fragment += "in flat int ID;\n\n";

    header_fragment += "vec2 uv = UV;\n\n";
    header_fragment += "vec4 color = vec4(0);\n\n";

    int model_id = 0;

    for (auto file : effectors)  header_fragment += Effector::source(file)+";\n\n";

    header_fragment += "void tic() { COLOR += color; uv = UV; color = vec4(1); }\n\n";
    header_fragment += "void tac() { COLOR += color; uv = UV; color = vec4(0); }\n\n";

    header_fragment += comment_line;

    body_fragment.clear();

    if (dc) for (auto &model : dc->models) {

        for (int instance = 0; instance < model.get()->s.quantity(); instance++) {

            auto name = model.get()->s.name();

            if (model.get()->s.quantity() > 1) name += "["+std::to_string(instance)+"]";

            body_fragment += "\t// "+name+"\n"; // would love this to be a node name instead // still matters ?
            body_fragment += "\ttic();\n"; // would love this to be a node name instead // still matters ?

            for (auto &effector : model.get()->effectors) {

                std::string arg_str;

                for (auto &arg : Effector::get(effector.get()->file).args) {

                    arg_str += dc->s.name()+"."+lower(name)+"."+effector->ref.name()+"."+arg.second+", ";

                }

                arg_str.resize(arg_str.size()-2);

                body_fragment += "\t"+effector->file->name()+"("+arg_str+");\n";
            }

        }

        body_fragment += "\ttac();\n\n";

        body_fragment += "\n";

        model_id++;
    }

    if (dc) {

        for (auto &effector : dc->effectors) {

            std::string arg_str;

            for (auto &arg : Effector::get(effector.get()->file).args) {

                arg_str += dc->s.name()+"."+effector->ref.name()+"."+arg.second+", ";

            }

            arg_str.resize(arg_str.size()-2);

            body_fragment += "\t"+effector->file->name()+"("+arg_str+");\n";
        }

        if (dc->effectors.size()) body_fragment += "\ttac();\n\n";

    }

}

void Layer::ShaderProgramBuilder::vert() {

    header_vertex.clear();

    header_vertex += "layout (location = 0) in vec2 POSITION;\n";
    header_vertex += "layout (location = 1) in vec2 TEXCOORD;\n";
    header_vertex += "layout (location = 3) in int OBJ;\n\n";

    header_vertex += "out vec2 UV;\n\n";
    header_vertex += "out int ID;\n\n";

    body_vertex.clear();
    body_vertex += "\tUV.y = 1-UV.y;\n\n";
    body_vertex += "\tID = gl_InstanceID;\n\n";
    body_vertex += "\t// vec2 pos = POSITION*layer[ID].size+layer[ID].pos;\n\n";

    body_vertex += "\tgl_Position = vec4(POSITION.x,POSITION.y,0,1);\n\n";

}



///////// DRAWCALL ////

DrawCall::DrawCall(std::string name = "") : s(name.length()?name:"layer") { engine.dynamic_ubo.add(&s); }

void DrawCall::draw() {

    shader.use();

    vbo.draw();

}

bool DrawCall::removeEffector(Effector* effector) {

    return std::erase_if( effectors, [&](std::shared_ptr<Effector> e) { return e.get() == effector; });

}


Effector* DrawCall::addEffector(File* file) {

    if (file->extension != "glsl") { PLOGW << "WARUM :GLSL only BB !!"; return nullptr;}

    auto effector = effectors.emplace_back(std::make_shared<Effector>(file, s.next_name(file->name()))).get();

    s.add(&effector->ref);

    return effector;

}

Model* DrawCall::add(File* f) {

    auto mod = models.emplace_back(std::make_shared<Model>(f, s.next_name(f->name()))).get();

    s.add(&mod->s);

    vbo.add(f, s.size()) ;

    return mod;

}

bool DrawCall::remove(Model* model){ return std::erase_if( models, [&](std::shared_ptr<Model> e) { return e.get() == model; }); }

void DrawCall::update() {

    static std::filesystem::file_time_type last_modified = std::chrono::file_clock::now();

    static bool has_changed = false;

    for (auto x : models) { // rien a foutre la

        if (x.get()->file->owned) continue;

        auto last_ = std::filesystem::last_write_time(std::filesystem::path(File::loc()) / x.get()->file->path);

        if (last_modified  < last_) { last_modified = last_; has_changed = true; }

    } /// ???

    if (has_changed) {

        int i = 0;

        vbo.reset();

        for (auto &x : models) vbo.add_noupload(x.get()->file, i++) ;

        vbo.upload();

        has_changed = false;

    }

    Layer::ShaderProgramBuilder builder(this);

    shader.create(builder.fragment, builder.vertex);

}
