#include "atlas.hpp"  



Atlas::Atlas(std::string path, int width, int height)  : binpack(width,height), ubo("mediasCoords", 100)  {

    init();

    texture.create(&data[0],width,height,0,0,1);

    texture.mipmaps = 10;

   Directory dir(path);

    // add dir.list.sort.by bigger px count (w*h) first HERE

    for (auto file:dir.list) { 

        Image img(dir.path+file);

        if (!add(img.width,img.height,&img.data[0])) std::cout << "need new atlas for that" <<std::endl;
    
    }

    int xx = 0;
    for (auto r:binpack.GetUsedRectangles()) std::cout << dir.list[xx++] << " " << r.width << " " <<  r.height << " " <<  r.x << " " <<  r.y  << std::endl;
  
}

void Atlas::link(ShaderProgram* shader) {

    ubo.data.resize(binpack.GetFreeRectangles().size()*4*4);

    ubo.link(shader->id);

    ubo.update(&normalized_list[0], binpack.GetFreeRectangles().size()*4*4);

    shader->sendUniform("mediasAtlas", 1);

}

void Atlas::init() {

    // buffered = true;

    data.resize(texture.width*texture.height*3); 

}
 
bool Atlas::add(int width, int height, unsigned char* data){ 

    for (auto cell : binpack.GetFreeRectangles() )  {

        if (width < cell.width && height < cell.height) {

            auto r = binpack.Insert(
                
                width, height,
                0,
                rbp::GuillotineBinPack::RectBestAreaFit, 
                rbp::GuillotineBinPack::SplitShorterAxis
            
            );

            normalized_list.push_back({r.width/(float)this->texture.width, r.height/(float)this->texture.height, r.x/(float)this->texture.width, r.y/(float)this->texture.height});
            
            texture.update(&data[0],r.width,r.height,r.x,r.y);

            return true;

        }

    }

    return false;

}