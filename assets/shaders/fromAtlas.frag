vec4 fromAtlas(vec2 uv, int id) { 
           
    uv *= mediaCoord[id].size;
    uv += mediaCoord[id].pos;

    return texture(mediasAtlas,uv);
 
}