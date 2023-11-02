#version 430 core

out vec4 color;

uniform sampler2D pass;
uniform sampler2D mediasAtlas;
flat in int obj;
flat in int id;
in vec2 texcoord;

struct Framebuffer { int width, height, x, y;  };
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

    int framebuffer;
    int canva_first;
    int canva_count;
    int fixture_first;

};

layout (binding = 2, std140) uniform mediasCoords { Rect[16] mediaCoord;};

layout (binding = 0, std140) uniform dynamic_ubo { Fixture fix[24]; Fixture fix2[24]; };
layout (binding = 1, std140) uniform static_ubo { Framebuffer framebuffer[100]; Mat mat[24]; Layer layer[10]; };

void main() {

    // color = vec4(0,texcoord.x,1,1); return;

    color = vec4(0);
  
    // if (obj == 0) return;

    for (int i = 0; i < layer[obj].canva_count; i++) {
        
        vec2 uv = texcoord;
        
        if (layer[obj].canva_count>1) {

            int current_canva = layer[obj].canva_first+i;
            uv = uv*mat[current_canva].size+mat[current_canva].pos;
        
        }

        color += texture(pass, uv);
        
    }       

    return;
    
}

