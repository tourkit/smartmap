#version 430 core

out vec4 color;

struct Cell {

    vec2 pos, size;
    
} cell;

in float id;
in vec2 texcoord;
uniform float x = 0;

void main() {

    if (id > 0){
        
        color = vec4(abs(texcoord.x*2-1));
    }else{

        color = vec4(1,0,0,1);

    }
    return;

}
