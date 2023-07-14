// FRAGMENT SHADER

#version 430 core

vec2 size = vec2(.2);
vec2 pos = vec2(.2);

out vec4 color;



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

float calculateBoxSDF(vec2 point, vec2 boxRect)
{
   vec2 delta = abs(point) - boxRect;
   return length(max(delta, 0.0)) + min(max(delta.x,delta.y),0.0); 
}


vec2 rotate(vec2 v, float a) {

    a *= 6.28318530718;

    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);

    return m*v;

}


void main() { 


    vec2 uv = (2.0 * gl_FragCoord.xy) / vec2(400, 300) - vec2(1);

    uv = rotate(uv,debug0);

    float distField = calculateBoxSDF(uv, size);


    distField = sign(distField);
    // distField = sign(distField);



    color = vec4(distField,0,0, 1.0);

}