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

Layer::Layer(uint16_t width, uint16_t height) : fb(width, height) { vbo.add(quad); }

Layer::Layer() : fb(engine.window.width,engine.window.height) {}

void Layer::draw() {

    fb.bind();

    DrawCall::draw();

}

DrawCall::DrawCall() {
}

void DrawCall::draw() {

    // engine.atlas->texture->bind();

    shader.use();

    vbo.draw();

}

void DrawCall::update() {

    static std::filesystem::file_time_type last_modified = std::chrono::file_clock::now();

    static bool has_changed = false;

    for (auto &x : vbo.models) {

        auto last_ = std::filesystem::last_write_time(std::filesystem::path(File::REPO_DIR) / x.file->path);

        if (last_modified  < last_) { last_modified = last_; has_changed = true; }

    }

    if (has_changed) {

        vbo.reloadFiles();

        has_changed = false;

    }

    shader.create(&vbo);

    engine.atlas->link(&shader);

}
