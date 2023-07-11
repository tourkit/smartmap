#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 2) in vec2 CLIPRECT;
layout (location = 3) in int OBJ;

struct Rect { vec2 size, pos; };

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

layout(std140, binding = 1) uniform MatriceUBO  { Rect mat[24]; };
layout(std140, binding = 2) uniform FixtureUBO  { Fixture fix[24];} ;

flat out int obj;
flat out int id;
out vec2 texcoord;

uniform float debug[10];


vec2 rotate(vec2 v, float a, vec2 r2) {

    // return v;
    
    float ratio = 2*r2.x/r2.y;

    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);
    
    vec2 AR = vec2(1.222222222);//1.0f+(1.0f-((1920.0f/1080.0f)-1.0f)));

    if (ratio > 1.) {   AR.x = ratio;  return (m*(v*AR))*(1./AR);  } 

    else { AR.y = ratio; return (m*(v/AR))/(1./AR); }

}

void main() {
    obj = OBJ;

    id = gl_InstanceID;

    texcoord = TEXCOORD;
    
    gl_Position = vec4(POSITION.x,POSITION.y,0,1);
    
    gl_ClipDistance[0] = 1;
    gl_ClipDistance[1] = 1;
    gl_ClipDistance[2] = 1;
    gl_ClipDistance[3] = 1;
// return;
    if (obj == 0) return;

    if (obj  == 2){

        vec2 size = mat[id].size;
        size *= fix[id].size;

        vec2 pos = fix[id].pos;
        pos *= mat[id].size+size;
        pos +=  mat[id].pos;


        gl_Position.xy = rotate(gl_Position.xy,fix[id].orientation,size);

        gl_Position.xy *= size;
        gl_Position.xy += pos;
        return;

        vec2 mins = mat[id].pos-mat[id].size;
        vec2 maxs = mat[id].pos+mat[id].size;
        gl_ClipDistance[0] = gl_Position.x-mins.x; 
        gl_ClipDistance[1] = maxs.x-gl_Position.x;
        gl_ClipDistance[2] = gl_Position.y -mins.y;
        gl_ClipDistance[3] = maxs.y-gl_Position.y;


    } else if (obj  == 1) {

        gl_Position.xy *= mat[id].size;
        gl_Position.xy += mat[id].pos;

        texcoord = gl_Position.xy*.5+.5;

    }
    

}
