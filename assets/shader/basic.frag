
#version 430 core

out vec4 color;

in vec2 uv;

uniform sampler2D buff;
uniform sampler2D mediasAtlas;

uniform float debug0 = 0;
uniform float debug1 = 0;
uniform float debug2 = 0;
uniform float debug3 = 0;
uniform float debug4 = 0;
uniform float debug5 = 0;
uniform float debug6 = 0;
uniform float debug7 = 0;
uniform float debug8 = 0;
uniform float debug9 = 0;

vec2 rotate(vec2 v, float a) {

    a *= 6.28318530718;

    float s = sin(a);
    float c = cos(a);

    return v * mat2(c, -s, s, c);
}

vec2 iResolution = vec2(400,200);

vec2 rectangle(vec2 texcoord, vec2 size, vec2 pos, float angle) {

    vec2 AR = vec2(1.);

    float ratio = iResolution.x/iResolution.y; // screen size;

    if (ratio > 1.) AR.x = ratio;
    else AR.y = ratio;

    texcoord -= pos; 
    texcoord = rotate(texcoord*AR,angle)*(1./AR);
    texcoord /= size;
    texcoord += .5;
    
    if (texcoord.x > 1. || texcoord.y > 1. || texcoord.x < 0. || texcoord.y < 0. ) return vec2(0.);
    
    return texcoord;
    
}


flat in int id;

struct Rect { vec2 size,pos; };



float col = 1./2;

Rect mat[3] = {


    Rect(vec2(col),vec2(0*col)),
    Rect(vec2(col),vec2(1*col)),
    Rect(vec2(col),vec2(2*col))

};


void main() { 

    vec2 size = vec2(debug0,1);
    vec2 pos = vec2(debug1,.5);

    if(id == 0) {

        size=vec2(1);
        pos=vec2(.5);

    }

    pos*=vec2(1+size.x,1);
    pos -= vec2(size.x*.5,0);

    size*=mat[id].size;
    pos*=mat[id].size;
    pos+=mat[id].pos;

    float angle = 0;

    vec2 t_uv = gl_FragCoord.xy/iResolution.xy;
    t_uv = sign(rectangle(t_uv, size, pos, angle))*.5; 
    color = vec4(t_uv,0,1); 


    // color = texture(buff,uv)+vec4(uv.x,0,0,1)+texture(mediasAtlas,uv);

}