#version 430 core

out vec4 color;

in vec2 tex;

void main() {
    
    color = vec4(1,tex.x,0,1);

}