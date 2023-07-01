#version 430 core

uniform sampler2D buff;
uniform sampler2D mediasAtlas;

out vec4 color;

in vec2 tex;

void main() { 

    // gl_FragCoord.xy;

    color = vec4(tex,0,1)+texture(buff, tex.xy)+texture(mediasAtlas, tex.xy);
    // color = vec4(.2);

}