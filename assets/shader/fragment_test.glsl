#version 430 core

out vec4 color;

uniform sampler2D pass;
uniform sampler2D mediasAtlas;

in float passID;
in float instanceID;
in vec2 texcoord;

uniform float feedback = 0;
uniform float texchoice = 0;
uniform vec2 test = vec2(0);
uniform vec2 merde = vec2(1);

layout (std140) uniform mediasCoords { vec4[16] media;};

vec4 fromAtlas(float pos) { return texture(mediasAtlas,texcoord*media[int(pos)].xy+media[int(pos)].zw); }

void main() {


    if (passID == 2) color = fromAtlas(0);

    else color = texture(pass,texcoord);
    
    if (passID == 1) color *= feedback;

    // color = vec4(1);

    return;

}
