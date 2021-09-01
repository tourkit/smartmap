#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in int uid;

struct Rect { vec2 size, pos; };

layout(std140) uniform MatriceUBO  { Rect matrice[10];} ;

out float UID;      // caps mean cast from int
out float INSTANCE; // caps mean cast from int
out vec2 texcoord;

uniform vec2 u_scale = vec2(1);
uniform vec2 u_translate = vec2(0);

void main() {

    UID = uid;

    int instance = gl_InstanceID;
    INSTANCE = instance;
    
    vec2 pos = position;

    Rect fixture;
    
    fixture.size = vec2(1);
    fixture.pos = vec2(0);
    
    if (instance == 0) {
        fixture.size = u_scale;
        fixture.pos = u_translate;
    }

    fixture.pos *= matrice[instance].size;
    fixture.pos += (1-fixture.size)*matrice[instance].size*.5;

    texcoord = pos;
    pos *= matrice[instance].size;

    if (UID  == 2){

        vec2 size = matrice[instance].size*fixture.size;
        pos *= fixture.size;
        texcoord = pos/size;
        pos += fixture.pos;

        vec2 ipos = min(pos,matrice[instance].size);
        texcoord *= (size-(pos-ipos))/size;
        pos = ipos;

        vec2 opos = max(pos, 0);
        texcoord += (opos-pos)/size;
        pos = opos;

    } else {
        
        texcoord *= matrice[instance].size;
        texcoord += matrice[instance].pos;
   
    }
    
    pos += matrice[instance].pos;

    if (UID == 0){ pos = position; texcoord = pos;}

    gl_Position = vec4(pos*2-1,0,1);

}
