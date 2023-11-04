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

layout (binding = 0, std140) uniform dynamic_ubo { Fixture fix[24]; Fixture fix2[24]; };
layout (binding = 1, std140) uniform static_ubo { Framebuffer framebuffer[100]; Mat mat[24]; Layer layer[10]; };

uniform sampler2D pass;
uniform sampler2D mediasAtlas;

vec2 rotate(vec2 v, float a) {

    float s = sin(a);
    float c = cos(a);

    return v * mat2(c, -s, s, c);
}

vec2 rectangle(vec2 uv, vec2 size, vec2 pos, float angle, vec2 AR) {

    pos *= 1+size;
    pos -= size*.5;
    uv -= pos; 
    // uv = rotate(uv*AR,angle)*(1./AR);
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


 
void main() {

        vec2 uv = texcoord;

        int fix_id = id+layer[obj].fixture_first;
        int canva_id = id+layer[obj].canva_first;
                
        color = vec4(0);
        // if (id==0) {color=vec4( 1); return;}
        color += texture(pass,(uv)*mat[canva_id].size+mat[canva_id].pos)*.9;

        // could be in matrice or some UBO ?
        vec2 AR = vec2(1,.5625);
        
        vec2 pos = fix[fix_id].pos;
        pos.y = 1-pos.y;

        vec2 outuv = rectangle(uv, fix[fix_id].size, pos, fix[fix_id].orientation, AR);
        float steps = 12;
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
            feedback_smoothing -= step*.0001;
            
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

        else { 
            
            if (gobo_id == 8) { color = rgba*fromAtlas(outuv, int(fix[fix_id].gobo[1]*15)); }
        }

        if (fix[fix_id].gobo[0] > .5) color = vec4(1)-color;

        // if (fix[fix_id].strobe>0) color *= mod(strobe,2+(1-fix[fix_id].strobe)*20); 

        color *= feedback_smoothing;

    return;

}