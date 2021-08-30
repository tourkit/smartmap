#version 430 core

layout (location = 0) in vec2 position;

struct Cell {

    vec2 pos; vec2 size;
    
} cell;


void main() {

    cell.pos = vec2(.10,0);
    cell.size = vec2(.1,1);

    vec2 pos = position;
    pos *= cell.size;
    pos += cell.pos;
    // pos += gl_InstanceID*.0;

    gl_Position = vec4(pos,0,1);

}
