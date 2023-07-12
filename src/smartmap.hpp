#pragma once

#include "window.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "atlas.hpp"
#include "ubo.hpp"
#include "gui.hpp"
#include "artnet.hpp"

using Fixture = std::vector<Artnet::Universe::Attribute>;

struct SmartMap {

 GLint GL_BLEND_MODE_IN = 6;
 GLint GL_BLEND_MODE_OUT = 13;
 std::vector<GLenum> GL_BLEND_MODES = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,	
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_CONSTANT_COLOR,
    GL_ONE_MINUS_CONSTANT_COLOR,
    GL_CONSTANT_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA,
    GL_SRC_ALPHA_SATURATE,
    GL_SRC1_COLOR,
    GL_ONE_MINUS_SRC1_COLOR,
    GL_SRC1_ALPHA,
    GL_ONE_MINUS_SRC1_ALPHA

};

    std::vector<float> debuguniforms{0,0,0,0,0,0,0,0,0,0};

    bool debug = true;

    Artnet *artnet;

    Window *window;

    VBO *quadA, *quadB, *quadC;

    Texture *passBuf, *outBuf, *outBlur;

    FrameBuffer *outFB, *winFB;

    ShaderProgram *basic, *shader, *blur_x, *blur_y;

    GUI *gui;

    UBO *matriceUBO, *fixtureUBO; 

    uint16_t FW, FH, MAT_X, MAT_Y;

    Atlas *atlas;

    float time,fps;

    void createLayer(GLuint chan, GLuint uni, Fixture *fixture, int count=1, float mode=0);

    static SmartMap& getInstance();

    void render();

private:

    SmartMap();
    SmartMap(const SmartMap&) = delete;
    SmartMap& operator=(const SmartMap&) = delete;
};
