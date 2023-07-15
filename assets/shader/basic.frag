
#version 430 core

out vec4 color;

in vec2 uv;

uniform sampler2D buff;
uniform sampler2D mediaAtlas;

void main() { 

    color = vec4(texture(buff,uv).r);//+texture(mediaAtlas,uv)+vec4(uv.x,0,0,1);

}