#version 430 core

struct Quad0 {

   vec4 rgba0;  
   
};


layout (binding = 0, std140) uniform dynamic_ubo { vec4 v; };

out vec4 color;

void main() { 
     
   color = vec4(v.x,v.y,v.z,1);    

}

 