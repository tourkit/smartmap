#version 430 core

layout (location = 0) in vec2 POS;
layout (location = 1) in vec2 TEX;
// layout (location = 2) in vec2 CLIPRECT;
// layout (location = 3) in int OBJ;

out vec2 tex;

void main() {

    tex = TEX;

    gl_Position = vec4(POS.x,POS.y,0,1);

}
