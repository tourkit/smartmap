#version 430 core

out vec4 color;

flat in int obj;
flat in int id;
in vec2 texcoord;

struct Framebuffer { int width, height, x, y;  };
struct Rect { vec2 size;vec2 pos;  };
struct Mat { vec2 size;vec2 norm;vec2 pos;  vec2 xxxxalign; };
struct Fixture {

    float alpha;
    float r;
    float g;
    float b;
    vec2 pos;
    vec2 size;
    vec4 gobo;
    float orientation; 
    float feedback; 
    float strobe;  
    float ratio; 

};
struct Layer {

    int framebuffer;
    int canva_first;
    int canva_count;
    int fixture_first;

};

layout (binding = 2, std140) uniform mediasCoords { Rect[16] mediaCoord;};

layout (binding = 0, std140) uniform dynamic_ubo { Fixture fix[50]; Fixture fix2[50]; int seq[4];  };
layout (binding = 1, std140) uniform static_ubo { Framebuffer framebuffer[100]; Mat mat[100]; Layer layer[10]; };

uniform sampler2D pass;
uniform sampler2D mediasAtlas;
uniform sampler2D freetype;


uniform int frame_sequence = 0;

vec2 rotate(vec2 v, float a) {

    float s = sin(a);
    float c = cos(a);

    return v * mat2(c, -s, s, c);
}

vec2 rectangle(vec2 uv, vec2 size, vec2 pos, float angle, vec2 AR) {

    pos *= 1+size;
    pos -= size*.5;
    uv -= pos; 
    uv = rotate(uv*AR,-angle)*(1./AR);
    uv /= size;
    uv += .5;
    
    if (uv.x > 1. || uv.y > 1. || uv.x < 0. || uv.y < 0. ) return vec2(0.);
    
    return uv;
    
}

vec4 fromAtlas(vec2 uv, int id) { 
           
    uv *= mediaCoord[id].size;
    uv += mediaCoord[id].pos;

    return texture(mediasAtlas,uv);
 
 }


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

    int id = layer[obj].fixture_first;
    float FBratio = .1;384/540; framebuffer[id].width/framebuffer[id].height;

    float rx = min((rows/columns)/FBratio,1.);
    float ry = min((columns/rows)*FBratio,1.);

    float x = l(uv.x,columns, thickness*rx);
    float y = l(uv.y,rows,thickness*ry);

   return clamp((x+y), 0.0, 1.0);


}

float grid(vec2 uv, float thickness, float columns, float rows, vec2 AR) {

    vec2 max_lines = vec2(10);
    // return uv.x+uv.y;

    vec2 grid = vec2(columns,rows);
    grid = 1.0+grid*max_lines;

    vec2 res = AR / grid ; 

    
    vec2 aspect = vec2(min(1.0,res.y/res.x),min(1.0,res.x/res.y)); 
    
    vec2 vthickness = vec2(thickness);
    vthickness *= aspect;

    vec2 pixel = uv;
    pixel = abs(pixel*2.0-1.0);    
    pixel*= grid;
    pixel = abs(mod(pixel,2.0)-1.0);
    pixel = step(pixel,vthickness); 

    float o = min(1.0,pixel.x+pixel.y);
  
    return o;

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

void main() {

        vec2 uv = texcoord;

        int fix_id = id+layer[obj].fixture_first;
        int canva_id = id+layer[obj].canva_first;
                
        color = vec4(0);
        // if (id==0) {color=vec4( 1); return;}
        if (fix[fix_id].feedback>0) color += vec4(texture(pass,(uv)*mat[canva_id].size+mat[canva_id].pos).xyz,0)-(.999999-(1-pow(fix[fix_id].feedback*.87+1,-10))*.999999);

        // could be in matrice or some UBO ?
        
        vec2 AR = mat[canva_id].xxxxalign;
        
        vec2 pos = fix[fix_id].pos;
        pos.y = 1-pos.y;

        vec2 outuv = rectangle(uv, fix[fix_id].size, pos, fix[fix_id].orientation, AR);
        float steps = 10;
        float feedback_smoothing = 1;
        if (fix[fix_id].feedback != 0) for (float i = 1; i < steps; i++)  {
        
            if (outuv.x+outuv.y>0) break;

            float angle = fix[fix_id].orientation;
            vec2 size = fix[fix_id].size;
            vec2 pos = fix[fix_id].pos;
            pos.y = 1-pos.y;
            vec2 pos2 = fix2[fix_id].pos;
            pos2.y = 1-pos2.y; 
            float step = i/steps;

            // feedback_smoothing -= step*debug0; // should be based on frame distance , maybe abs(pos.x-fix2[fix_id].pos.x) ?
            feedback_smoothing -= step*.00231;
            
            // // 3.14159265359 // 6.2831853072
            if (abs(angle-fix2[fix_id].orientation)<.25) angle = mix(angle,fix2[fix_id].orientation,step);
            if (abs(size.x-fix2[fix_id].size.x)<.015 && abs(size.y-fix2[fix_id].size.y)<.015) size = mix(size,fix2[fix_id].size,step);
            if (abs(pos.x-pos2.x)<.12 && abs(pos.y-pos2.y)<.12) pos = mix(pos,pos2,step);

            outuv = rectangle(uv, size, pos, angle, AR); 
            
        }

        if (outuv.x+outuv.y==0) return;

        // outuv.y = 1-outuv.y; // flip for buffers


        vec4 rgba = vec4(fix[fix_id].r,fix[fix_id].g,fix[fix_id].b,fix[fix_id].alpha)*fix[fix_id].alpha;

        int gobo_id = int(fix[fix_id].gobo[0]*255)%127;

        if (gobo_id == 0) { color = rgba*vec4(1); }
        

        else { if (gobo_id == 10) { color += rgba*texture(freetype,(outuv*mat[canva_id].size*.99+mat[canva_id].pos)).r; }

        else { if (gobo_id == 8) { color += rgba*fromAtlas(outuv, int(fix[fix_id].gobo[1]*15)); }
        
        else {     if (gobo_id == 1) { color += rgba*grid(outuv, fix[id].gobo[1], fix[id].gobo[2], fix[id].gobo[3], AR*fix[fix_id].size); }

        else {

            outuv *= .0666; // dunno why gotta do this, works for all ....
            outuv *=2;
            outuv -=1;

            if (gobo_id == 2) color = rgba*grid2(outuv, fix[id].gobo[1], fix[id].gobo[2], fix[id].gobo[3]);
            else if (gobo_id == 3) color = rgba*gradient(outuv, fix[id].gobo[1], fix[id].gobo[2], fix[id].gobo[3]);
            else if (gobo_id == 4) color = rgba*burst(rotate(outuv, fix[id].gobo[3]), fix[id].gobo[1], 0, fix[id].gobo[2]);
            else if (gobo_id == 5) color = rgba*burst(rotate(outuv, fix[id].gobo[3]), fix[id].gobo[1], 1, fix[id].gobo[2]);
            else if (gobo_id == 6) color = rgba*flower(outuv*(1/fix[id].size), fix[id].gobo[1], fix[id].gobo[2], fix[id].gobo[3]);
            else if (gobo_id == 7) color = rgba*border(outuv, fix[id].gobo[1]);
            else if (gobo_id == 9) color = rgba*s1plx(outuv, fix[id].gobo[1], fix[id].gobo[2], fix[id].gobo[3]);

        }}}}

        if (fix[fix_id].gobo[0] > .5) color = vec4(1)-color;

        // if (fix[fix_id].strobe>0) color *= mod(seq[0],2); 
        // if (fix[fix_id].strobe>0) color *= mod(seq[0],2+(1-fix[fix_id].strobe)*500); 

        color *= feedback_smoothing;

    return;

}