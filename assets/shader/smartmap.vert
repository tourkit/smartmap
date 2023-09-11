#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 2) in vec2 RESOLUTION;
layout (location = 3) in int OBJ;

struct Test { vec2 pos, size; };

struct Fixture {

    float alpha;
    float r;
    float g;
    float b;
    vec2 pos;
    vec2 size;
    vec4 gobo;
    float orientation; 
    float feedback; 
    float strobe;  
    float ratio; 

};

layout (binding = 0, std140) uniform dynamic_ubo { Fixture test[2]; };

void main() {
    
    gl_Position = vec4(POSITION.xy,0,1);


}
