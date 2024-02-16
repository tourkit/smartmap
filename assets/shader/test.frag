#version 430 core

layout (binding = 0, std140) uniform dynamic_ubo {  vec4 x; };


out vec4 color;

    
void main() {  

    color = vec4(x.x,x.y,x.z+.2,1); return;
             
}              