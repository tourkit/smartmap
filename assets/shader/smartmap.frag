#version 430 core

out vec4 color;

uniform sampler2D pass; 

flat in int obj;
flat in int inst;
in vec2 texcoord;


void main() {

    color = texture(pass,texcoord);

    return;

}

