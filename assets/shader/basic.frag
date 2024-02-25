#version 430 core
out vec4 color;

uniform sampler2D texture0;

in  vec2 uv;

void main() { 
     
   color = vec4(uv.x,uv.y,0,1)+texture(texture0,uv);    

}
