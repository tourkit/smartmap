
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

   vec2 uv = gl_FragCoord.xy/FBResolution.xy;

   vec2 max_lines = vec2(4);

   vec2 aspect_ractio = vec2(1.0);

   vec2 resolution = FBResolution.xy;
   resolution/=max_lines*vec2(debug0,debug1)+1;
   if (resolution.y < resolution.x) aspect_ractio =  vec2(resolution.y/resolution.x,1);
   else aspect_ractio = vec2(1,resolution.x/resolution.y);

   vec2 thickness = vec2(debug2);
   thickness *= aspect_ractio;

   thickness.x += debug3;
   // thickness.y -= debug3;

   vec2 lines_count = vec2(debug0,debug1)*(max_lines-2.0)+1.0;


   vec2 grid;
   grid = abs(uv*2.0-1.0);    
   grid*=(lines_count+thickness);
   grid = abs(mod(grid,2.0)-1.0);
   grid = step(grid,thickness); 


   color += vec4(0);
   float carre = debug5*FBResolution.y;
   float offset = debug6*FBResolution.y;
   if (

      gl_FragCoord.x > FBResolution.x-carre-offset && 
      gl_FragCoord.y > FBResolution.y-carre-offset &&
      gl_FragCoord.x < FBResolution.x-offset && 
      gl_FragCoord.y < FBResolution.y
      
   ) color-=vec4(1.8);

   // color += vec4(grid.x,grid.y,0,1); return; 
   color += vec4(vec3(grid.x+grid.y),1); 

    vec2 AR = vec2(1.);
    float ratio = FBResolution.x/FBResolution.y;
    if (ratio > 1.) AR.x = ratio;
    else AR.y = ratio;


}