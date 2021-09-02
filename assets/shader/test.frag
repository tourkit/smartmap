// DAS UBAR SHADAR

out vec4 color;

uniform sampler2D medias[num_atlases];

uniform float UID;
uniform float INSTANCE;

int uid, instance;

// global

struct CoordinatesUBO { vec2 size, pos; float ratio; } 
layout(std140) uniform Coordinates { CoordinatesUBO coords[nb_uids]; }; 

struct AttributesUBO { float attr[nb_attribtues]; } // wont (?hardly) work with instances
layout(std140) uniform Attributes { AttributesUBO attr[nb_attributes]; }; 

struct TexturesUBO { vec2 size, pos; int atlas; }
layout(std140) uniform Textures { TexturesUBO textures[nb_textures]; };  // always has: "O" => [{1,1},{0,0},0]

vec4 tex(int id = 0) { return texture(medias[textures[id].atlas],texcoord*textures[id].size+textures[id].pos); }

// local

struct UBO0 { vec4 color;}
struct UBO1 { vec4 size,pos; int id; }
struct UBO2 { float feedback; float strobe; }
layout(std140) uniform UBO { UBO0 ubo0[nb_instances]; UBO0 ubo1[nb_instances]; UBO0 ubo2[nb_instances]; };

void main() {

    uid = int(UID);
    inst = int(INSTANCE);

    if (uid == 0) {

        color = ubo0[inst].color;

    }else if (uid == 1) {

        texcoord *= ubo1[inst].size;
        texcoord += ubo1[inst].pos;
        color = tex(ubo1[inst].id);

    }else if (uid == 2) {

        color  = tex();
        color *= ubo2[inst].feedback;
        color *= ubo2[inst].strobe;

    }else{
    
        color = vec4(1);

    }

}