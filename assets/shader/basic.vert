#version 430 core

layout (location = 0) in vec2 POS;
layout (location = 1) in vec2 TEX;
layout (location = 2) in vec2 CLIPRECT;
layout (location = 3) in int OBJ;

out vec2 tex;
flat out int obj;
flat out int id;

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


void main() {

    tex = TEX;
    obj = OBJ;
    id = gl_InstanceID;

    gl_Position = vec4(POS.x,POS.y,0,1);

    gl_ClipDistance[0] = gl_Position.x+1-debug0*2;
    gl_ClipDistance[1] = gl_Position.y+1-debug0*2;
    gl_ClipDistance[2] = -gl_Position.x+1-debug1*2;
    gl_ClipDistance[3] = -gl_Position.y+1-debug1*2;
    
}
