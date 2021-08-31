#version 430 core

out vec4 color;

uniform sampler2D pass;
uniform sampler2D media;

in float passID;
in float instanceID;
in vec2 texcoord;

uniform float feedback = 0;

void main() {

    if (passID > 0) color = texture(media,texcoord);

    else color = texture(pass,texcoord)*feedback;

    if (passID == 999) color = texture(pass,texcoord);

    return;

}
