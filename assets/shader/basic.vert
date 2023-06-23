#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 2) in vec2 CLIPRECT;
layout (location = 3) in int OBJ;

void main() {

    obj = OBJ;

    id = gl_InstanceID;

    texcoord = TEXCOORD;
    
    gl_Position = vec4(POSITION,0,1);
    

}
