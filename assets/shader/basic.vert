// VERTEX SHADER

#version 430 core

layout (location = 0) in vec2 POS;
layout (location = 1) in vec2 TEX;
layout (location = 2) in vec2 RES;
layout (location = 3) in int OBJ;


void main() { 
    
    gl_Position = vec4(POS,0,1);
    
}