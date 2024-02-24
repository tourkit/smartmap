#version 430 core

layout (location = 0) in vec2 POSITION;

void main() {
    
    gl_Position = vec4(POSITION.x,POSITION.y,0,1);  
    
}