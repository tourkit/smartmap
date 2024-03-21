#version 430 core

///////////////////////////////////////////

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
out vec2 UV;


void main() {

	UV = TEXCOORD;
	gl_Position = vec4(POSITION.x,POSITION.y,0,1);

}