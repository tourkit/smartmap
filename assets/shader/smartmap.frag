#version 430 core

out vec4 color;

uniform sampler2D pass; 

flat in int obj;
flat in int id;
in vec2 texcoord;
in vec2 celcoord;

void main() {

    // if (min(vec2(0),celcoord) != celcoord) return;
    // if (max(vec2(1),celcoord) != celcoord) return;
    // color = texture(pass,texcoord);
    color = vec4(celcoord.x,1-celcoord.x,0,1);

    return;

}

