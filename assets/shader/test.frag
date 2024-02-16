#version 430 core

layout (binding = 0, std140) uniform dynamic_ubo {  float x[4]; };


out vec4 color;

    
void main() {  

    color = vec4(1,x[0],0,1); return;
             
}              