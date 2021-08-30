#version 430 core

out vec4 color;


struct Cell { vec2 size;vec2 pos;  } cell;
struct Fixture { vec2 size;vec2 pos;  } fixture;


uniform sampler2D media;

in float passID;
in float instanceID;
in vec2 texcoord;
uniform float x = 0;

void main() {

    if (passID > 0){
        
        color = texture(media,texcoord)*vec4(abs(texcoord.x*2-1));
    }else{

        color = texture(media,texcoord);//vec4(1,0,0,1);

    }
    return;

}
