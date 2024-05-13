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

Layer::Layer(uint16_t width, uint16_t height, std::string name)

    : fb((width?width:engine.window.width), (height?height:engine.window.height)), DrawCall(name) {

    feedback = new Texture(fb.width,fb.height,2,1, GL_RGB8);

}



void Layer::draw() {

    if (feedback) { feedback->bind(); }

    fb.bind();

    DrawCall::draw();

    if (feedback) { return feedback->read(fb.texture); }

}

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

    vbo.upload();

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

        for (auto &x : models) vbo.add(x.get()->file, i++) ;

        vbo.upload();

        has_changed = false;

    }

    shader.create(this);

}
