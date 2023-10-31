#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 2) in vec2 RESOLUTION;
layout (location = 3) in int OBJ;

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
layout (binding = 2, std140) uniform mediasCoords { Rect[16] mediaCoord;};

layout (binding = 0, std140) uniform dynamic_ubo { Fixture fix[24]; Fixture fix2[24]; };
layout (binding = 1, std140) uniform static_ubo { Mat mat[24];  };

flat out int obj;
flat out int id;
flat out vec2 FBResolution;

out vec2 texcoord;
out float FBratio;

uniform int offset = 0;

uniform float debug0 = 0;
uniform float debug1 = 0;
uniform float debug2 = 0;
uniform float debug3 = 0;
uniform float debug4 = 0;

void main() {

    FBratio = RESOLUTION.x/RESOLUTION.y;
    FBResolution = vec2(RESOLUTION.x, RESOLUTION.y);
    // if (obj == 3 || obj == 4) FBResolution*=4;

    obj = OBJ;

    id = gl_InstanceID+offset;
    // if (obj == 3) id+=2;
    // if (obj == 4) id+=2;

    texcoord = TEXCOORD;
    
    gl_Position = vec4(POSITION.x,POSITION.y,0,1);

    if (obj==0) return;

    gl_Position.xy *= mat[id].size;
    gl_Position.xy += mat[id].norm;

    if (mod(obj-1,2)  == 0) texcoord = gl_Position.xy*.5+.5;

}
