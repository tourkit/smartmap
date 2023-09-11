// VERTEX SHADER

#version 430 core

layout (location = 0) in vec2 POS;
layout (location = 1) in vec2 TEX;
layout (location = 2) in vec2 RES;
layout (location = 3) in int OBJ;

struct Rect { vec2 size;vec2 pos;  };
layout (binding = 0, std140) uniform static_ubo { Rect mat[24];  };
layout (binding = 1, std140) uniform dynamic_ubo {  Rect matrice2[24]; };

void main() { 
    
    gl_Position = vec4(POS,0,1);
    
}