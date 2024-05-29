#include "drawcall.hpp"
#include "effector.hpp"
#include "engine.hpp"
#include "layer.hpp"


DrawCall::DrawCall(std::string name = "") : s(name.length()?name:"layernonono") { engine.dynamic_ubo.add(&s); }

void DrawCall::draw() {

    shader.use();

    vbo.draw(s.quantity());

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
