// VERTEX SHADER

#version 430 core

layout (location = 0) in vec2 POS;
layout (location = 1) in vec2 TEX;

out vec2 uv;
flat out int id;


void main() { 
    
    uv = TEX;

    id = gl_InstanceID;

    gl_Position = vec4(POS.xy, 0, 1);
}