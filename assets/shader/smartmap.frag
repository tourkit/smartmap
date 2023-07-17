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


struct Rect { vec2 size,pos; };
float col = 1./2;

Rect mmat[4] = {

    Rect(vec2(col),vec2(0*col,0*col)),
    Rect(vec2(col),vec2(0*col,1*col)),
    Rect(vec2(col),vec2(1*col,0*col)),
    Rect(vec2(col),vec2(1*col,1*col))

};

vec4 smartmap(Fixture f) {

    vec2 t_uv = gl_FragCoord.xy/FBResolution.xy;
    // CLIP
    if ( 
        t_uv.x < mmat[id].pos.x  || t_uv.x > mmat[id].pos.x+mmat[id].size.x 
        || t_uv.y < mmat[id].pos.y|| t_uv.y > mmat[id].pos.y+mmat[id].size.y
    ) return vec4(0); 
    // return vec4(t_uv.x);

    vec2 size = f.size;
    vec2 pos = f.pos;
    float angle = debug0;
    // size = vec2(1);
    // pos = vec2(0.5);

    pos*=vec2(1)+size;
    pos -= size*.5;
    size*=mmat[id].size;
    pos*=mmat[id].size;
    pos+=mmat[id].pos;

    vec2 AR = vec2(1);

    t_uv = rectangle(t_uv, size, pos, angle, AR); 

    // return vec4(t_uv,0,1);

    // return vec4(t_uv,0,1);

    int gobo_id = int(f.gobo[0]*255);

    vec4 rgba = vec4(f.r,f.g,f.b,f.alpha)*f.alpha;
    return sign(t_uv.x)*rgba*vec4(1);

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
    
    // if (id!=0) return;
    
    if (obj == 1) {

        //  color = vec4(uv.x); return;
        color = vec4(0);
        // return;
        float feedback = 1;
        feedback -= fix[id].feedback;

        //if (!(fix[id].rgba.r == 0 && fix[id].rgba.g == 0 && fix[id].rgba.b == 0)) feedback -= 1-pow(abs((fix[id].feedback*.5+.5)-1),3);
        color = texture(pass,uv)*debug3; 
        
        return;

    }
    
    if (obj == 2) { 

        // color = vec4(uv.x); return;
        
        for (float i = 0; i < 1; i++)  {
            
            Fixture f = fix[id];

            // f.orientation = mix(fix[id].orientation,fix2[id].orientation,i/10.0);
            // f.size = mix(fix[id].size,fix2[id].size,i/10.0);
            // f.pos = mix(fix[id].pos,fix2[id].pos,i/10.0);
            // f.r = mix(fix[id].r,fix2[id].r,i/10.0);
            // f.g = mix(fix[id].g,fix2[id].r,i/10.0);
            // f.b = mix(fix[id].b,fix2[id].r,i/10.0);
            // f.alpha = mix(fix[id].alpha,fix2[id].r,i/10.0);

            color = smartmap(f);  
            
        }
        
    }

    return;

}

