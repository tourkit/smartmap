#version 430 core

layout (location = 0) in vec2 position;

struct Cell { vec2 size;vec2 pos;  } cell;
struct Fixture { vec2 size;vec2 pos;  } fixture;

out float id;
out vec2 texcoord;
uniform float x = 0; // from 0 to 1

void main() {

    id = gl_InstanceID;

    vec2 pos = position;

    if (id > 0){

        cell.size = vec2(.5,1);
        cell.pos = vec2(.25,0);

        fixture.size = vec2(1,1);
        fixture.pos = vec2(.0,.0);
        fixture.pos.x = x*2-1;
        fixture.pos.x *= cell.size.x;

        pos *= fixture.size;
        pos *= cell.size;
        texcoord = pos/cell.size;
        pos += cell.pos;
        pos += fixture.pos;

        vec2 ipos = min(pos, cell.pos+cell.size);
        texcoord *= (cell.size-(pos-ipos))/cell.size;
        pos = ipos;

        vec2 opos = max(pos, cell.pos);
        vec2 test = opos-pos;
        texcoord += test/cell.size;
        pos = opos;

    }

    gl_Position = vec4(pos*2-1,0,1);

}
