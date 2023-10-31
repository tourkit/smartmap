#version 430 core

out vec4 color;

uniform sampler2D tex;

flat in int obj;
flat in int id;
flat in vec2 FBResolution;

in vec2 texcoord;
in float FBratio;

void main() {

    color = texture(tex, texcoord); 
         
    return;
    
}

