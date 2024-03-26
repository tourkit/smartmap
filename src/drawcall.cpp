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

    // unbind old ahder ?

    shader.create(&vbo);

    engine.dynamic_ubo.bind(&shader);
    engine.static_ubo.bind(&shader);

    engine.atlas->link(&shader);

}