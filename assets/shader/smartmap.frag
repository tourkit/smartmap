#version 430 core

out vec4 color;

uniform sampler2D pass;
uniform sampler2D mediasAtlas;

flat in int obj;
flat in int id;
flat in vec2 FBResolution;

in vec2 texcoord;
in float FBratio;

uniform int fixcount = 1;

uniform float mode = 1;

uniform float feedback = 0;
uniform float texchoice = 0;

struct Rect { vec2 size;vec2 pos;  };
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

layout (binding = 0, std140) uniform mediasCoords { Rect[16] mediaCoord;};
layout(binding = 2, std140) uniform FixtureUBO  { Fixture fix[24];} ;
layout(binding = 3, std140) uniform FixtureUBO2  { Fixture fix2[24];} ;

layout(std140) uniform MatriceUBO  { Rect mat[24]; };
uniform int MatriceUBOSize = 1; // could move to matriceUBO a var called "size"

vec2 rotate(vec2 v, float a) {

    a *= 6.28318530718;

    float s = sin(a);
    float c = cos(a);

    return v * mat2(c, -s, s, c);
}



vec2 rectangle(vec2 uv, vec2 size, vec2 pos, float angle, vec2 AR) {

    uv -= pos; 
    uv = rotate(uv*AR,angle)*(1./AR);
    uv /= size;
    uv += .5;
    
    if (uv.x > 1. || uv.y > 1. || uv.x < 0. || uv.y < 0. ) return vec2(0.);
    
    return uv;
    
}


void main() {

    vec2 uv = gl_FragCoord.xy/FBResolution.xy;
    uv = texcoord;

    if (obj == 0) { 

        color = vec4(0);
        
        if (mode==1)  {color = texture(pass, uv); return;}

        for (int i = 0; i < MatriceUBOSize; i++) color += texture(pass, uv*mat[i].size+mat[i].pos);

        return;
         
    }
    
    // if (id!=1 && id!=2) return;
    
    if (mod(obj-1,2) == 0) {

        //  color = vec4(uv.x); return;

        color = texture(pass,uv)-(1-min(.998,fix[id].feedback)); 
        
        return;

    }
    
    if (mod(obj-1,2) == 1) { 

        // color = vec4(uv.x); return;
        

        float steps =15;

        vec2 outuv;

        Fixture f = fix[id];

        // if (fix[id].pos == fix2[id].pos) return;

        for (float i = 0; i < steps; i++)  {
        

            f.orientation = mix(fix[id].orientation,fix2[id].orientation,i/steps);
            f.size = mix(fix[id].size,fix2[id].size,i/steps);
            f.pos = mix(fix[id].pos,fix2[id].pos,i/steps);
            // f.r = mix(fix[id].r,fix2[id].r,i/steps);
            // f.g = mix(fix[id].g,fix2[id].r,i/steps);
            // f.b = mix(fix[id].b,fix2[id].r,i/steps);
            // f.alpha = mix(fix[id].alpha,fix2[id].r,i/steps);

            // CLIP
            if ( 
                uv.x < mat[id].pos.x  || uv.x > mat[id].pos.x+mat[id].size.x 
                || uv.y < mat[id].pos.y|| uv.y > mat[id].pos.y+mat[id].size.y
            ) continue; 

            vec2 size = f.size;
            vec2 pos = f.pos;
            float angle = debug0;
            // size = vec2(1);
            // pos = vec2(0.5);

            pos*=vec2(1)+size;
            pos -= size*.5;
            size*=mat[id].size;
            pos*=mat[id].size;
            pos+=mat[id].pos;

            vec2 AR = vec2(1);

            outuv += (rectangle(uv, size, pos, angle, AR)); 
            // uv  = sign(uv);



            
        }

        float sss = sign(outuv.x+outuv.y);
        
        vec4 rgba = vec4(f.r,f.g,f.b,f.alpha)*f.alpha;

        color = vec4(sss)*rgba*vec4(1);
        
    }

    return;

}

