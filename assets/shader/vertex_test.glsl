#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in int ID;

struct Cell { vec2 size;vec2 pos;  };
struct Fixture { vec2 size;vec2 pos;  } fixture;

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

    texcoord = pos;

    Cell cell[10];
    cell[0].size = vec2(1,.1);
    cell[0].pos = vec2(0,0);
    cell[1].size = vec2(1,.1);
    cell[1].pos = vec2(0,.1);
    cell[2].size = vec2(1,.1);
    cell[2].pos = vec2(0,.2);
    cell[3].size = vec2(1,.1);
    cell[3].pos = vec2(0,.3);
    cell[4].size = vec2(1,.1);
    cell[4].pos = vec2(0,.4);
    cell[5].size = vec2(1,.1);
    cell[5].pos = vec2(0,.5);
    cell[6].size = vec2(1,.1);
    cell[6].pos = vec2(0,.6);
    cell[7].size = vec2(1,.1);
    cell[7].pos = vec2(0,.7);
    cell[8].size = vec2(1,.1);
    cell[8].pos = vec2(0,.8);
    cell[9].size = vec2(1,.1);
    cell[9].pos = vec2(0,.9);

    fixture.size = u_scale;
    fixture.pos = u_translate;

    fixture.pos *= cell[gl_InstanceID].size;
    fixture.pos += (1-fixture.size)*cell[gl_InstanceID].size*.5;//cell[gl_InstanceID].pos;//*.5;

    pos *= cell[gl_InstanceID].size;

        
    if (passID > 0){

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

    if (passID == 999){ pos = position; texcoord = pos;}

    gl_Position = vec4(pos*2-1,0,1);

}
