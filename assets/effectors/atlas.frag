//id(0,10,0)
/////////////
void atlas(float id_) {

	int id = id_;

    uv *= media[id].size;
    uv += media[id].pos;

    color *= texture(medias, uv);

}

