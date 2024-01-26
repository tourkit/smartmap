vec2 rectangle(vec2 uv, vec2 size, vec2 pos, float angle, vec2 AR) {

    pos *= 1+size;
    pos -= size*.5;
    uv -= pos; 

    float s = sin(-angle);
    float c = cos(-angle);

    uv = uv*AR*mat2(c, -s, s, c);

    uv*=(1./AR);
    uv /= size;
    uv += .5;
    
    if (uv.x > 1. || uv.y > 1. || uv.x < 0. || uv.y < 0. ) return vec2(0.);
    
    return uv;
    
}