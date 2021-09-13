#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 3) in int ID;

struct Rect { vec2 size;vec2 pos; }, mat[12], fix[12];

flat out int obj;
flat out int id;
out vec2 texcoord;

uniform float ratio = 1;
uniform float count = 0;

vec2 rotate(vec2 v, float a, vec2 r2) {

    float ratio = 2*r2.x/r2.y;

    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);

    vec2 AR = vec2(1);
    if (ratio > 1.) { 
        AR.x = ratio; 
        return (m*(v*AR))*(1./AR); 
        
    } else { 
        AR.y = ratio; 
        return (m*(v/AR))/(1./AR); }

}

void main() {


    // DEBUG BEGIN

    fix[0].size = vec2(.95);
    fix[0].pos = vec2(0);
    fix[1].size = vec2(.95);
    fix[1].pos = vec2(0);
    fix[2].size = vec2(.5,.95);
    fix[2].pos = vec2(0);
    fix[3].size = vec2(.95);
    fix[3].pos = vec2(0);

    vec2 size = vec2(.333,1);
    mat[0].size = size;
    mat[0].pos = vec2(0);
    mat[1].size = size;
    mat[1].pos = vec2(-size.x*2,0);
    mat[2].size = size*vec2(1,.5);;
    mat[2].pos = vec2(size.x*2,.5*-size.y);
    mat[3].size = size*vec2(1,.5);;
    mat[3].pos = vec2(size.x*2,.5*size.y);


    // DEBUG END

    obj = ID;

    id = gl_InstanceID;

    texcoord = TEXCOORD;

    vec2 pos = POSITION;

    vec2 t_size = mat[id].size * fix[id].size;
    vec2 t_pos = mat[id].pos + fix[id].pos;

    pos = rotate(pos,count,t_size);

    vec4 ClipRect = vec4(vec2((1/fix[id].size.x),1/fix[id].size.y),vec2((1/-fix[id].size.x),1/(fix[id].size.y)));
    gl_ClipDistance[0] = pos.x+ClipRect[0];
    gl_ClipDistance[1] = ClipRect[1] - pos.x;
    gl_ClipDistance[2] = pos.y - ClipRect[2];
    gl_ClipDistance[3] = ClipRect[3] - pos.y;

    pos *= t_size;
    pos += t_pos;


    gl_Position = vec4(pos,0,1);



}
