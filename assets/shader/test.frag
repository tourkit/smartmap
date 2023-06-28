#version 430 core

uniform sampler2D buff;
uniform float lala;

// struct Fixture {

//     vec4 rgba;
//     vec2 pos;
//     vec2 size;
//     vec4 gobo;
//     float orientation; 
//     float feedback; 
//     float strobe;  
//     float ratio; 

// };

// layout(std140) uniform FixtureUBO  { Fixture fix[1];} ;

out vec4 color;

in vec2 tex;

void main() { 

    // gl_FragCoord.xy;
    
    vec4 p = texture(buff, tex.xy);
    // p *= vec2(.2,0)
    color = p;
    // color *= vec4(fix[0].rgba.r+lala,1,1,1);
    // color = vec4(.1);
    // color = vec4(0,1,tex.x,1);

}