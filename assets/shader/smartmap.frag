#version 430 core

out vec4 color;

uniform sampler2D pass;
uniform sampler2D mediasAtlas;

in float UID;
in float INSTANCE;
in vec2 texcoord;

uniform float feedback = 0;
uniform float texchoice = 0;

struct Rect { vec2 size;vec2 pos;  };
struct Fixture {

    vec2 size, pos;
    vec4 rgba;
    vec4 gobo;
    float feedback;
    float strobe;
    vec2 useless; // pack size for std140

};

layout(std140) uniform MatriceUBO  { Rect matrice[10]; };
layout (std140) uniform mediasCoords { Rect[16] mediaCoord;};
layout(std140) uniform FixtureUBO  { Fixture fixtures[10];} ;

vec4 fromAtlas(int id) { return texture(mediasAtlas,texcoord*mediaCoord[id].size+mediaCoord[id].pos); }


void main() {

    int instance = int(INSTANCE);

    if (UID == 0) { 

        for (int i = 0; i < 10; i++) color += texture(pass,texcoord*matrice[i].size+matrice[i].pos); 
        
    }

    else if (UID == 1) {color =  texture(pass,texcoord)*fixtures[instance].feedback; }

    else if (UID == 2) {color = fixtures[instance].rgba*fromAtlas(int(texchoice));}

    return;

}
