#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;

layout (binding = 0, std140) uniform dynamic_ubo { vec4 v; };

void main() {
    
    gl_Position = vec4(POSITION.x,POSITION.y,0,1);  
    
}