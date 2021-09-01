#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in int uid;

struct Rect { vec2 size, pos; };

struct Fixture {

    vec2 size, pos;

    vec4 rgba;

    vec4 gobo;

    float feedback;
    float strobe;

    vec2 useless; // pack size for std140

};

layout(std140) uniform MatriceUBO  { Rect matrice[10]; };
layout(std140) uniform FixtureUBO  { Fixture fixtures[10];} ;

out float UID;      // caps mean cast from int
out float INSTANCE; // caps mean cast from int
out vec2 texcoord;

void main() {

    UID = uid;

    int instance = gl_InstanceID;
    INSTANCE = instance;
    
    vec2 pos = position;

    Fixture fixture = fixtures[instance];

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
