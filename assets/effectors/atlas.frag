//id(0,10,0)

void atlas(int id) { 

    uv *= media[id].size;
    uv += media[id].pos;
    color *= texture(medias, uv);

}