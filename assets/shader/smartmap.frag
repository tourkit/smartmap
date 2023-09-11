#version 430 core

out vec4 color;

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

    color = vec4(1,1,test[1].size.x,1); 
   
    return;

}

