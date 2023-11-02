#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 3) in int OBJ;

struct Framebuffer { int width, height, x, y;  };
struct Rect { vec2 size;vec2 pos;  };
struct Mat { vec2 size;vec2 norm;vec2 pos; vec2 xxxxalign; };

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
    float ratio; // unused for now mostly for alignment

};
struct Layer {

    int framebuffer;
    int canva_first;
    int canva_count;
    int fixture_first;

};
layout (binding = 2, std140) uniform mediasCoords { Rect[16] mediaCoord;};

layout (binding = 0, std140) uniform dynamic_ubo { Fixture fix[24]; Fixture fix2[24]; };
layout (binding = 1, std140) uniform static_ubo { Framebuffer framebuffer[100]; Mat mat[24]; Layer layer[10]; };

flat out int obj;
flat out int id;

out vec2 texcoord;

void main() {

    texcoord = TEXCOORD;
    obj = OBJ;
    id = gl_InstanceID;

    
    gl_Position = vec4(POSITION.x,POSITION.y,0,1);  


    
    int current = layer[obj].canva_first + id;
    gl_Position.xy *= mat[current].size;
    gl_Position.xy += mat[current].norm;

    // if (mod(obj-1,2)  == 0) texcoord = gl_Position.xy*.5+.5;

}
