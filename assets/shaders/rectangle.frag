//size(0,1,1) 
//pos(-1,1,0) 
//angle(-1,1,0)    


void rectangle(vec2 size, vec2 pos, float angle) {

    pos *= 1+size;
    pos -= size*.5;
    uv -= pos; 
    //uv = rotate(uv*AR,-angle)*(1./AR);
    uv /= size;
    uv -= .5;
    
    if (uv.x > 1. || uv.y > 1. || uv.x < 0. || uv.y < 0. ) uv = vec2(0);
    
}
  
//yo 

vec2 rotate(vec2 v, float a) {

    float s = sin(a);
    float c = cos(a);

    return v * mat2(c, -s, s, c);
}