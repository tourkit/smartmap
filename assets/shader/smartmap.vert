#version 430 core

layout (location = 0) in vec2 POSITION;
layout (location = 1) in vec2 TEXCOORD;
layout (location = 2) in int ID;

struct Rect { vec2 size;vec2 pos; }, mat[12], fix[12];

flat out int obj;
flat out int id;
out vec2 texcoord;
out vec2 celcoord;

uniform float count = 0;

vec2 rotate(vec2 v, float a, vec2 r2) {

    float t_ratio = 2*r2.x/r2.y;

    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);

    vec2 AR = vec2(1);
// return (m*(v*AR))*(1./AR); 
    if (t_ratio > 1.) { 
        AR.x = t_ratio; 
        return (m*(v*AR))*(1./AR); 
        
    } else { 
        AR.y = t_ratio; 
        return (m*(v/AR))/(1./AR); }

}

void main() {

    obj = ID;

    id = gl_InstanceID;

    texcoord = TEXCOORD;

    vec2 pos = POSITION;

    // DEBUG BEGIN

    float t = 1./3;
    float ht = 1./6;

    mat[0].size = vec2(t,1);
    mat[0].pos = vec2(-1+ht,0);
    mat[1].size = vec2(t,1);
    mat[1].pos = vec2(t+ht,0);
    mat[2].size = vec2(.5);
    mat[2].pos = vec2(-.5,.5);
    mat[3].size = vec2(.5);
    mat[3].pos = vec2(.5,-.5);


    fix[0].size = vec2(.95);
    fix[1].size = vec2(.95);
    fix[2].size = vec2(.95);
    fix[3].size = vec2(.95);


    // mat[0].size = vec2(.5,1);
    // mat[0].pos = vec2(-.5,0);
    // mat[1].size = vec2(.5,1);
    // mat[1].pos = vec2(.5,0);

    vec2 size = vec2(.333,1);
    mat[0].size = size;
    mat[0].pos = vec2(0);
    mat[1].size = size;
    mat[1].pos = vec2(-size.x*2,0);
    mat[2].size = size*vec2(1,.5);;
    size.y = .5;
    mat[2].pos = vec2(size.x*2,-size.y);
    mat[3].size = size;
    mat[3].pos = vec2(size.x*2,size.y);


    // DEBUG END

        // fixture.pos *= matrice[instance].size;
        // fixture.pos += (1-fixture.size)*matrice[instance].size*.5;

    vec2 t_size = mat[id].size * fix[id].size;
    vec2 t_pos = mat[id].pos + fix[id].pos;

    texcoord = pos ;
    texcoord *= t_size;
    texcoord += t_pos;
    texcoord*=.5;
    texcoord+=.5;

    pos = rotate(pos,count,mat[id].size);
    celcoord = pos;
    pos *= t_size;
    pos += t_pos;

    gl_Position = vec4(pos,0,1);


// 
    // texcoord = POSITION*.125+.125;
}
