#pragma once

#include "engine.hpp"

#include "atlas.hpp"
#include "artnet.hpp"

#include "ndi.hpp"

#include "smartmap/config.hpp"

namespace SmartMap {
    
struct Base {

    static inline Atlas *atlas;

    Base();

    static inline Artnet *artnet;
    static inline ShaderProgram *shader, *layershader;

    static inline Buffer::Object *fix1UBO;
    static inline Buffer::Object *fix2UBO;
    static inline Buffer::Object *smartlayersUBO;

    Config config;

    Stack stack;

    GLint GL_BLEND_MODE_IN = 11;
    GLint GL_BLEND_MODE_OUT = 13;
    GLint GL_BLEND_MODE_IN2 = 1;
    GLint GL_BLEND_MODE_OUT2 = 1;
    GLint GL_BLEND_MODE_IN3 = 1;
    GLint GL_BLEND_MODE_OUT3 = 1;
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
        // GL_SRC1_COLOR,
        // GL_ONE_MINUS_SRC1_COLOR,
        // GL_SRC1_ALPHA,
        // GL_ONE_MINUS_SRC1_ALPHA

    };


};
};