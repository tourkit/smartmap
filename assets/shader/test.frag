#version 430 core

uniform sampler2D buff;
uniform sampler2D mediaAtlas;

out vec4 color;

in vec2 tex;

void main() { 

    // gl_FragCoord.xy;

    color = texture(buff, tex.xy)+texture(mediaAtlas, tex.xy);
    // color = vec4(.2);

}