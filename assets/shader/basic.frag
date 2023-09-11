
#version 430 core

out vec4 color;

struct Rect { vec2 size;vec2 pos;  };
layout (binding = 0, std140) uniform static_ubo { Rect mat[24];  };
layout (binding = 1, std140) uniform dynamic_ubo {  Rect matrice2[24]; };


void main() { 

   color = vec4(mat[0].pos.x+matrice2[0].pos.x);
   

}

