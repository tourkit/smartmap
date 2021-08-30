#version 430 core

layout (location = 0) in vec2 position;
// layout (location = 1) in float id;

void main() {

    vec2 pos = position;
    // pos *= .5;
    // pos += gl_InstanceID*.0;

    gl_Position = vec4(pos,0,1);

}
