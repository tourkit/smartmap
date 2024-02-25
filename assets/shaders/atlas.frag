//atlas(0,10,0)

void atlas(int id) { 

    int t_id = 0;

    uv *= mediascoords[t_id].size;
    uv += mediascoords[t_id].position;
    color *= texture(mediasAtlas, uv);

}