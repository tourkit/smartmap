
#version 430 core

out vec4 color;

in vec2 uv;

uniform sampler2D buff;
uniform sampler2D mediasAtlas;

void main() { 

    color = texture(buff,uv)+vec4(uv.x,0,0,1)+texture(mediasAtlas,uv);

}