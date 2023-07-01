#version 430 core

out vec4 color;

uniform sampler2D pass;
uniform sampler2D mediasAtlas;

flat in int obj;
flat in int id;
in vec2 texcoord;

uniform float fixcount = 1;

uniform float feedback = 0;
uniform float texchoice = 0;

struct Rect { vec2 size;vec2 pos;  };
struct Fixture {

    vec4 rgba;
    vec2 pos;
    vec2 size;
    vec4 gobo;
    float orientation; 
    float feedback; 
    float strobe;  
    float ratio; 

};

layout (binding = 0, std140) uniform mediasCoords { Rect[16] mediaCoord;};
layout(binding = 2, std140) uniform FixtureUBO  { Fixture fix[24];} ;

layout(std140) uniform MatriceUBO  { Rect mat[24]; };
uniform int MatriceUBOSize = 1; // could move to matriceUBO a var called "size"

vec4 fromAtlas(vec2 uv, int id) { 
    
    // id /= 10
    uv = texcoord;
    uv *= mediaCoord[id].size;
    uv += mediaCoord[id].pos;
    // uv -= texcoord;
    // uv += .5;

    

    return vec4(uv,0,1)+texture(mediasAtlas,uv);
 
 }

float t_ratio = 1;

float l(float val, float count, float thickness) {

   float o = count*.5*thickness;

   val = val*(1.+o)-o; // border
   // v = v*(1.-c)-c; // border less

   val = (mod(val, count)*(1./count)+thickness);//*step(count,1.-val);

   val = step(1.0,val); // cross mode

    return val;

}

float grid2(vec2 uv, float thickness, float columns, float rows) {

    uv = abs(texcoord-.5)*2.;

    columns = (1-columns)*.8+.2;
    rows = (1-rows)*.8+.2;
    thickness = 1-thickness;

    float rx = min((rows/columns)/t_ratio,1.);
    float ry = min((columns/rows)*t_ratio,1.);

    float x = l(uv.x,columns, thickness*rx);
    float y = l(uv.y,rows,thickness*ry);

   return clamp((x+y), 0.0, 1.0);


}

float grid(vec2 uv, float thickness, float columns, float rows) {


    thickness = 1-thickness;

   columns = clamp(columns*10., 0.5, 100.);
   rows = clamp(rows*10., 0.5, 100.);

    columns = 1./(columns+(thickness*.5));
   rows = 1./(rows+thickness*.5);

   float tx = thickness*min((rows/columns)/t_ratio,1.);
   float ty = thickness*min((columns/rows)*t_ratio,1.);


   uv = 1.-abs(uv)*2.;

   float o = 0.;

   o += step(mod(uv.x,columns)/columns,tx);
   o += step(mod(uv.y,rows)/rows,ty);

   return clamp(o, 0.0, 1.0);

}

vec4 border(vec2 t_uv, float thickness) {

    t_uv+=.5;

    thickness /= 2.;

    float o = t_uv.x;

    if (t_uv.x < thickness || t_uv.x > 1.0-thickness) o = 1.0-t_uv.x;
    if (t_uv.y < thickness || t_uv.y > 1.0-thickness) o = 1.0-t_uv.x;


    return vec4(o);

}

vec4 flower(vec2 t_uv, float inratio, float shape, float petals) {

     float c = length(t_uv)+.5;

     float a = atan(t_uv.x,t_uv.y);//+iTime;

     float r = a * ((petals*29.)+3.);//  - rotation*3.14*2.;

     r = abs(fract(.5/3.14*r)-.5)*2.;

     r = pow(r,.5+pow(shape*2.,4.)); // petal type

     r = 1.-r*inratio*.5;//; // spikes out

     r = step(c,r);

    return vec4(r);

}

vec2 rotate(vec2 v, float a) {

    a *= 6.28318530718;

    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);

    return m*v;

}

float burst(vec2 uv, float inratio, float shape, float petals) {

    shape = 1-shape;


        float c = length(uv)+.5;

     float a = atan(uv.x,uv.y);//+iTime;

     float r = a * ((petals*29.)+3.);//  - rotation*3.14*2.;


      r = abs(fract(.5/3.14*r)-.5)*2.; // equals : abs(mod(.5/3.14*r,1.)-.5); equals sdTriangle(a,petals);


      r = clamp(r*c*.9,0.,1.);

      r += (1.-inratio*2.); // gradient to white

      float zzz = shape*.4999;
      r = smoothstep(zzz,1.-zzz, r);

      r = mix(0.,1.,r);

        return r;
}

vec4 gradient(vec2 uv, float aaaa, float inratio, float angle) {

    uv = rotate(uv, angle);
    uv += .5;

    uv += 1-aaaa*2;

    return vec4(uv.x);


}

vec3 random3(vec3 c) {
    float j = 4096.0 * sin(dot(c, vec3(17.0, 59.4, 15.0)));
    vec3 r;
    r.z = fract(512.0 * j);
    j *= 0.125;
    r.x = fract(512.0 * j);
    j *= 0.125;
    r.y = fract(512.0 * j);
    j *= 0.125;
    return r - 0.5;
}

const float F3 = 0.333333333333;
const float G3 = 0.166666666667;

float simplex3d(vec3 p) {
    // step 1. find current tetrahedron T and its 4 vertices
    // s, s + i1, s + i2, s + 1.0: skewed coordinates of T
    // x, x1, x2, x3: unskewed coordinates relative to T

    // calc s x
    vec3 s = floor(p + dot(p, vec3(F3)));
    vec3 x = p - s + dot(s, vec3(G3));

    // calc i1 i2
    vec3 e = step(vec3(0.0), x - x.yzx);
    vec3 i1 = e * (1.0 - e.zxy);
    vec3 i2 = 1.0 - e.zxy * (1.0 - e);

    // x1, x2, x3
    vec3 x1 = x - i1 + G3;
    vec3 x2 = x - i2 + 2.0 * G3;
    vec3 x3 = x - 1.0 + 3.0 * G3;

    // step2. find 4 surflets
    vec4 w;
    vec4 d;

    // calc weights
    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);

    // w fades from 0.6 at center to 0.0 at margin
    w = max(0.6 - w, 0.0);

    // calc surflet components
    d.x = dot(random3(s), x);
    d.y = dot(random3(s + i1), x1);
    d.z = dot(random3(s + i2), x2);
    d.w = dot(random3(s + 1.0), x3);

    // multiply d by w^4
    w *= w;
    w *= w;
    d *= w;
    return dot(d, vec4(52.0));
}
    
vec4 s1plx(vec2 uv, float height, float zoom, float contrast) {
    return vec4(pow(simplex3d(vec3(uv * (101.0 - zoom * 100.0), height * 5.0)), 1.0 + contrast * 8.0));
}


vec4 smartmap(int instance) {


    vec2 t_uv = texcoord-.5;

    int gobo_id = int(fix[instance].gobo[0]*255);

    if (gobo_id == 1) return fix[instance].rgba*grid(t_uv, fix[instance].gobo[1], fix[instance].gobo[2], fix[instance].gobo[3]);
    if (gobo_id == 2) return fix[instance].rgba*grid2(t_uv, fix[instance].gobo[1], fix[instance].gobo[2], fix[instance].gobo[3]);
    if (gobo_id == 3) return fix[instance].rgba*gradient(t_uv, fix[instance].gobo[1], fix[instance].gobo[2], fix[instance].gobo[3]);
    if (gobo_id == 4) return fix[instance].rgba*burst(rotate(t_uv, fix[instance].gobo[3]), fix[instance].gobo[1], 0, fix[instance].gobo[2]);
    if (gobo_id == 5) return fix[instance].rgba*burst(rotate(t_uv, fix[instance].gobo[3]), fix[instance].gobo[1], 1, fix[instance].gobo[2]);
    if (gobo_id == 6) return fix[instance].rgba*flower(t_uv*(1/fix[instance].size), fix[instance].gobo[1], fix[instance].gobo[2], fix[instance].gobo[3]);
    if (gobo_id == 7) return fix[instance].rgba*border(t_uv, fix[instance].gobo[1]);
    if (gobo_id == 8) return fix[instance].rgba*fromAtlas(t_uv, int(fix[instance].gobo[1]*10));
    if (gobo_id == 9) return fix[instance].rgba*s1plx(t_uv, fix[instance].gobo[1], fix[instance].gobo[2], fix[instance].gobo[3]);

    return fix[instance].rgba*vec4(1);


}

void main() {

    
        // color = vec4(.5);return;

    if (obj == 0) { 

        color = vec4(0);
        for (int i = 0; i < MatriceUBOSize; i++) color += texture(pass, texcoord*mat[i].size+((mat[i].pos+(1-mat[i].size))*.5) );  
         
    }
    else if (obj == 1) {

        float feedback = 1;

        // missing fix[id].beam[0]!= 0 // for ????
        if (!(fix[id].rgba.r == 0 && fix[id].rgba.g == 0 && fix[id].rgba.b == 0)) feedback -= 1-pow(abs((fix[id].feedback*.5+.5)-1),3);
        
        color =  texture(pass,texcoord)-max(.002,feedback); 
        

    }
    else if (obj == 2) { color = smartmap(id);  }

    return;

}

