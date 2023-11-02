#version 430 core

out vec4 color;

uniform sampler2D pass;
uniform sampler2D mediasAtlas;
flat in int obj;
flat in int id;
in vec2 texcoord;

struct Res { int width, height;  };
struct Rect { vec2 size;vec2 pos;  };
struct Mat { vec2 size;vec2 norm;vec2 pos;  vec2 xxxxalign; };
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
struct Layer {

    int width;
    int height;
    int fixture_first;
    int fixture_count;
    int canva_first;
    int canva_count;
    float ratio;
    float xxx1; // alignment

};

layout (binding = 2, std140) uniform mediasCoords { Rect[16] mediaCoord;};

layout (binding = 0, std140) uniform dynamic_ubo { Fixture fix[24]; Fixture fix2[24]; };
layout (binding = 1, std140) uniform static_ubo { Res framebuffer[100]; Mat mat[24]; Layer layer[10]; };

void main() {

    color = vec4(0);

    for (int i = 0; i < layer[obj].canva_count; i++) {
        
        int current_canva = layer[obj].canva_first+i;
        
        color += texture(pass, texcoord*mat[current_canva].size+mat[current_canva].pos);
        
    }       

    return;
    
}

