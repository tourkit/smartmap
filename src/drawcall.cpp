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

    // tofix 
    engine.dynamic_ubo.bind(&shader);
    engine.static_ubo.bind(&shader);

    // engine.atlas->link(&shader);

}

void DrawCall::draw() {
 
    shader.sendUniform("medias", 1);
    engine.atlas->texture->bind();
    vbo.draw();

    shader.use(); 

}

void DrawCall::update() {

    ShaderProgram::Builder shader_builder(&vbo);

    shader.create(shader_builder.frag(),shader_builder.vert());

}