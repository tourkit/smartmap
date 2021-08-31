#version 430 core

out vec4 color;

uniform sampler2D pass;
uniform sampler2D mediasAtlas;

in float passID;
in float instanceID;
in vec2 texcoord;

uniform float feedback = 0;
uniform float texchoice = 0;

layout (std140) uniform mediasCoords { vec4[16] rect;};

vec4 fromAtlas(float pos) { vec4 media[1]; media[0] = vec4(0.0944824219,0.0944824219,0,0); return texture(mediasAtlas,texcoord*media[int(pos)].xy+media[int(pos)].zw); }

void main() {

    if (passID > 0) color = fromAtlas(0);

    else color = texture(pass,texcoord)*feedback;

    if (passID == 999) color = texture(pass,texcoord);

    // color = vec4(1);

    return;

}
