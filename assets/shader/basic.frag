// FRAGMENT SHADER

#version 430 core

vec2 size = vec2(.5);
vec2 pos = vec2(0);

out vec4 color;

uniform float debug0 = 0;
uniform float debug1 = 0;
uniform float debug2 = 0;
uniform float debug3 = 0;
uniform float debug4 = 0;
uniform float debug5 = 0;
uniform float debug6 = 0;
uniform float debug7 = 0;
uniform float debug8 = 0;
uniform float debug9 = 0;

vec2 rotate(vec2 v, float a) {

    a *= 6.28318530718;

    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);

    return m*v;

}

float window_width = 400;
float window_height = 200;

void main() { 

    vec2 uv = (2.0 * gl_FragCoord.xy - vec2(window_width, window_height)) / vec2(window_width, window_height);
    
    // float angle = debug0;
    uv += pos * (1.0 + size);
    // uv = rotate(uv,angle);

    vec2 clampedUV = clamp(uv, -size, size) / (2.0 * size) + 0.5;

    vec2 delta = abs(uv) - size;
    float box = sign(-(length(max(delta, vec2(0.0))) + min(max(delta.x, delta.y), 0.0)));

    color = vec4(clampedUV, 0.0, 1.0) * box;

}