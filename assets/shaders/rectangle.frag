//pos(-1,1,0) 
//angle(-1,1,0) 

void rectangle(vec2 size, vec2 pos, float angle, vec2 AR) {

    pos *= 1+size;
    pos -= size*.5;
    uv -= pos; 
    //uv = rotate(uv*AR,-angle)*(1./AR);
    uv /= size;
    uv += .5;
    
    if (uv.x > 1. || uv.y > 1. || uv.x < 0. || uv.y < 0. ) uv = vec2(0);
    
}