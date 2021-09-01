#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in int ID;

struct Cell { vec2 size;vec2 pos;  };
struct Fixture { vec2 size;vec2 pos;  } fixture;

layout(std140) uniform cellsUBO  { Cell[10] cell;} ;

out vec2 merde;

out float passID;
out float instanceID;
out vec2 texcoord;
uniform float x = 0; // from 0 to 1

uniform vec2 u_scale = vec2(1);
uniform vec2 u_translate = vec2(0);

void main() {


    vec2 pos = position;
    
    passID = ID;
    instanceID = gl_InstanceID;

    fixture.size = u_scale;
    fixture.pos = u_translate;

    fixture.pos *= cell[gl_InstanceID].size;
    fixture.pos += (1-fixture.size)*cell[gl_InstanceID].size*.5;

    texcoord = pos;
    pos *= cell[gl_InstanceID].size;

    if (passID  == 2){

        vec2 size = cell[gl_InstanceID].size*fixture.size;
        pos *= fixture.size;
        texcoord = pos/size;
        pos += fixture.pos;

        vec2 ipos = min(pos,cell[gl_InstanceID].size);
        texcoord *= (size-(pos-ipos))/size;
        pos = ipos;

        vec2 opos = max(pos, 0);
        texcoord += (opos-pos)/size;
        pos = opos;

    } else {
        
        texcoord *= cell[gl_InstanceID].size;
        texcoord += cell[gl_InstanceID].pos;
   
    }
    
    pos += cell[gl_InstanceID].pos;

    if (passID == 0){ pos = position; texcoord = pos;}

    gl_Position = vec4(pos*2-1,0,1);

}
