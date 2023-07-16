
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

flat in vec2 FBResolution;

flat in float FBratio;

flat in int id;
flat in int obj;
struct Rect { vec2 size,pos; };


vec2 size = vec2(debug0,debug1);
vec2 pos = vec2(debug2,debug3);


vec2 rectangle(vec2 texcoord, vec2 size, vec2 pos, float angle, vec2 AR) {

    texcoord -= pos; 
    texcoord = rotate(texcoord*AR,angle)*(1./AR);
    texcoord /= size;
    texcoord += .5;
    
    if (texcoord.x > 1. || texcoord.y > 1. || texcoord.x < 0. || texcoord.y < 0. ) return vec2(0.);
    
    return texcoord;
    
}



struct Rect { vec2 size,pos; };
float col = 1./3;

Rect mat[3] = {

    Rect(vec2(col),vec2(0*col,0*col)),
    Rect(vec2(col),vec2(0*col,1*col)),
    Rect(vec2(col),vec2(1*col,1*col))

};

void main() { 
// color = vec4(1,0,0,1); return; 
    vec2 uv = gl_FragCoord.xy/FBResolution.xy;
    if (obj==0) { color = texture(buff, uv); return; }

    // if (obj==1) { color = vec4(1,0,0,1); return; }


    if(id == 0) { color = vec4(vec3(uv.y+uv.x)*.5,1); }

    // CLIP
    if ( uv.x < mat[id].pos.x  || uv.x > mat[id].pos.x+mat[id].size.x || uv.y < mat[id].pos.y|| uv.y > mat[id].pos.y+mat[id].size.y) return; 

    if(id != 1) {

        size=vec2(1);
        pos=vec2(.5);

    }

    pos*=vec2(1)+size;
    pos -= size*.5;
    size*=mat[id].size;
    pos*=mat[id].size;
    pos+=mat[id].pos;


    vec2 AR = vec2(1.);
    float ratio = FBResolution.x/FBResolution.y;
    if (ratio > 1.) AR.x = ratio;
    else AR.y = ratio;


    float angle = 0;



    uv = sign(rectangle(uv, size, pos, angle, AR))*.5; 

    color += vec4(uv,0,1); 


    // color = texture(buff,uv)+vec4(uv.x,0,0,1)+texture(mediasAtlas,uv);

}