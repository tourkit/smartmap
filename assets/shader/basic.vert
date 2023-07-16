// VERTEX SHADER

#version 430 core

layout (location = 0) in vec2 POS;
layout (location = 1) in vec2 TEX;
layout (location = 2) in vec2 RES;
layout (location = 3) in int OBJ;

out vec2 uv;
flat out int id;
flat out int obj;
flat out vec2 iResolution2;
flat out float FBratio;


void main() { 
    
    uv = TEX;
    obj = OBJ;

    FBratio = RES.x/RES.y;

    iResolution2 = vec2(RES.x, RES.y);
    
    id = gl_InstanceID;

    gl_Position = vec4(POS.xy, 0, 1);
    
}