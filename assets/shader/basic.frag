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

    return v * mat2(c, -s, s, c);
}

vec2 iResolution = vec2(400,200);

vec4 rectangle(vec2 texcoord, vec2 size, vec2 pos, float angle) {

    vec2 AR = vec2(1.);

    float ratio = iResolution.x/iResolution.y; // screen size;

    if (ratio > 1.) AR.x = ratio;
    else AR.y = ratio;

    texcoord -= pos; 
    texcoord = rotate(texcoord*AR,angle)*(1./AR);
    texcoord /= size;
    texcoord += .5;
    
    if (texcoord.x > 1. || texcoord.y > 1. || texcoord.x < 0. || texcoord.y < 0. ) return vec4(0.);
    
    return vec4(1);
    
}

float window_width = 400;
float window_height = 200;

void main() { 

    vec2 size = vec2(.1, .5);
    
    vec2 pos = vec2(.5, .5);
 
    float angle = debug0;

    vec2 texcoord = gl_FragCoord.xy/iResolution.xy;

    for (int i = 0; i < 10000; i++) color = rectangle(texcoord,size,pos,angle);

}