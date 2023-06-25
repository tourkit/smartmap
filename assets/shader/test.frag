#version 430 core

uniform sampler2D buff;

out vec4 color;

in vec2 tex;

void main() { 

    // gl_FragCoord.xy;
    
    vec4 p = texture(buff, tex.xy);
    // p *= vec2(.2,0)
    color = p;
    // color = vec4(.1);
    // color = vec4(0,1,tex.x,1);

}