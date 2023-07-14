// VERTEX SHADER

#version 430 core

layout (location = 0) in vec2 POS;

void main() { 
    gl_Position = vec4(POS.xy, 0, 1);
}