#version 430 core

uniform sampler2D buff;
uniform sampler2D mediasAtlas;

out vec4 color;

flat in int obj;
flat in int id;
in vec2 tex;

void main() { 

    // color = vec4(0,0,0,1); return; 

    if (obj == 2) { 
        
        float r = 0;
        if (id == 0) { r=1; }
        float g = 0;
        if (id == 1) { g=1; }
        color = vec4(r,g,0,1); return; 

    }

}