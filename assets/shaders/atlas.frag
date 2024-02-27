//atlas(0,10,0)

void atlas(int id) { 

    int t_id = 0;

    uv *= media[t_id].size;
    uv += media[t_id].pos;
    color *= texture(mediasAtlas, uv);

}