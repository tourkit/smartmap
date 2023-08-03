
#version 430 core

out vec4 color;

in vec2 uv;

uniform sampler2D buff;

uniform float columns = 0;
uniform float rows = 0;
uniform float thickness = 0;
uniform float debugsquare = 0;
uniform float debug4 = 0;
uniform float debug5 = 0;
uniform float debug6 = 0;
uniform float debug7 = 0;
uniform float debug8 = 0;
uniform float debug9 = 0;

flat in vec2 FBResolution;

flat in float FBratio;

flat in int id;
flat in int obj;

vec2 max_lines = vec2(2);

void main() { 

    vec2 grid = vec2(columns,rows);
    grid = 1.0+grid*max_lines;

    // grid.x +=  debug4;  
    // grid.y +=  debug5;  

    vec2 res = FBResolution.xy / grid ;
    vec2 aspect = vec2(min(1.0,res.y/res.x),min(1.0,res.x/res.y)); 
    
    vec2 thickness = vec2(thickness);
    thickness *= aspect;

    vec2 pixel = uv;
    pixel = abs(pixel*2.0-1.0);    
    pixel*= grid;
    pixel = abs(mod(pixel,2.0)-1.0);
    pixel = step(pixel,thickness); 

    // color = vec4(pixel.x,pixel.y,0,1); 
    // color = vec4(min(1.0,pixel.x)); 
    // color = texture(buff,pixel); 
    color = vec4(min(1.0,pixel.x+pixel.y)); 


    // DEBUGSQUARE

    float carre = debugsquare*FBResolution.y;
    vec2 offset1 = vec2(debug6,debug7)*FBResolution.y;
    if (

        gl_FragCoord.x > FBResolution.x-carre-offset1.x && 
        gl_FragCoord.y > FBResolution.y-carre-offset1.y &&
        gl_FragCoord.x < FBResolution.x-offset1.x && 
        gl_FragCoord.y < FBResolution.y-offset1.y

    ) color-=vec4(.8);
    carre*=.5;
    vec2 offset2 = vec2(debug8,debug9)*FBResolution.y;
    if (

        gl_FragCoord.x > FBResolution.x-carre-offset2.x && 
        gl_FragCoord.y > FBResolution.y-carre-offset2.y &&
        gl_FragCoord.x < FBResolution.x-offset2.x && 
        gl_FragCoord.y < FBResolution.y-offset2.y

    ) color-=vec4(.8);


}

