#version 430 core

layout (binding = 0, std140) uniform dynamic_ubo {  int seq[4]; };


out vec4 color;

    
void main() {  

    color = vec4(1,0,0,1); return; 
     
}