#version 430 core

layout (location = 0) in vec2 POS;
layout (location = 1) in vec2 TEX;

layout(binding = 1, std140) uniform aaa { vec4 a1,a2,a3,a4;} ;
layout(binding = 2, std140) uniform bbb { vec4 b1,b2,b3,b4;} ;


out vec2 tex;

void main() {

    tex = TEX;

    gl_Position = vec4(POS.x+b1.x,POS.y+a1.x,0,1);

}
