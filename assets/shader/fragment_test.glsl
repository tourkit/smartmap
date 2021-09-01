#version 430 core

out vec4 color;
// out vec4 feddpass;

uniform sampler2D pass;
uniform sampler2D mediasAtlas;

in float passID;
in float instanceID;
in vec2 texcoord;

uniform float feedback = 0;
uniform float texchoice = 0;

struct Rect { vec2 size;vec2 pos;  };

layout (std140) uniform mediasCoords { Rect[16] mediaCoord;};

vec4 fromAtlas(int id) { return texture(mediasAtlas,texcoord*mediaCoord[id].size+mediaCoord[id].pos); }

struct Fixture { float feedback; };

void main() {

    Fixture f;
    f.feedback = 0;

    if (instanceID == 0) f.feedback = feedback;

    if (passID == 2) color = fromAtlas(int(texchoice));

    else color = texture(pass,texcoord);
    
    if (passID == 1) color *= f.feedback;

    // color = vec4(1);
    // feddpass = color;

    return;

}
