#version 430 core

out vec4 color;

uniform sampler2D pass;

flat in int obj; // could be layer_id
flat in int id; // could be fixture_id

in vec2 uv;

struct Rect { vec2 size, pos; };

struct Layer {

    int fixture_first;
    int fixture_count;
    int canva_first;
    int canva_count;
    int width;
    int height;
    float ratio;
    float xxx1,xxx2; // alignment

};

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
    float xxcxxx; // unused for now mostly for alignment

};

layout (binding = 0, std140) uniform dynamic { Fixture fixture[24]; Fixture fix2[24]; };
layout (binding = 1, std140) uniform static {  Layer[10] layer; Rect canva[1000];  };

void main() {

    int layer_id = obj;

    color = vec4(0);

    for (int i = 0; i < layer[layer_id].canva_count; i++) {
        
        int current_canva = layer[layer_id].canva_first+i;
        
        color += texture(pass, uv*canva[current_canva].size+canva[current_canva].pos);
        
    }       

    return;
    
}

