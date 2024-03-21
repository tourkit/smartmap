#version 430 core

///////////////////////////////////////////

in vec2 UV;

out vec4 COLOR;


void main() {

	COLOR = vec4(UV.x);

} 