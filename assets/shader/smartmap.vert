#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 2) in int ID;

struct Rect { vec2 size;vec2 pos; }, mat[12], fix[12];

flat out int obj;
flat out int id;
out vec2 texcoord;
out vec2 celcoord;

uniform float count = 0;

vec2 rotate(vec2 v, float a) {

    float t_ratio = 2;

    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);

    vec2 AR = vec2(1);

    if (t_ratio > 1.) { AR.x = t_ratio; return (m*(v*AR))*(1./AR); } else { AR.y = t_ratio; return (m*(v/AR))/(1./AR); }

}

void main() {

    obj = ID;

    id = gl_InstanceID;

    texcoord = TEXCOORD;

    // DEBUG BEGIN

    float t = 1./3;
    float ht = 1./6;

    mat[0].size = vec2(t,1);
    mat[0].pos = vec2(-1+ht,0);
    mat[1].size = vec2(t,1);
    mat[1].pos = vec2(t+ht,0);
    mat[2].size = vec2(.5);
    mat[2].pos = vec2(-.5,.5);
    mat[3].size = vec2(.5);
    mat[3].pos = vec2(.5,-.5);


    fix[0].size = vec2(.95);
    fix[1].size = vec2(.95);
    fix[2].size = vec2(.95);
    fix[3].size = vec2(.95);


    // mat[0].size = vec2(.5,1);
    // mat[0].pos = vec2(-.5,0);
    // mat[1].size = vec2(.5,1);
    // mat[1].pos = vec2(.5,0);

    vec2 size = vec2(.333,1);
    mat[0].size = size;
    mat[0].pos = vec2(0);
    mat[1].size = size;
    mat[1].pos = vec2(-size.x*2,0);
    mat[2].size = size;
    mat[2].pos = vec2(size.x*2,0);
    mat[3].size = vec2(0);
    mat[3].pos = vec2(0);


    // DEBUG END

    vec2 pos = POSITION;
    pos *= fix[id].size;
    pos *= mat[id].size;

    texcoord = pos+mat[id].pos;
    texcoord *= mat[id].size*.5;
    texcoord += mat[id].size*.5;
    texcoord /= mat[id].size;

    pos = rotate(pos,count);
    celcoord = pos;//-vec2(.3333,0);fix[id].size;

    pos += mat[id].pos;

    gl_Position = vec4(pos,0,1);


// 
    // texcoord = POSITION*.125+.125;
}
