#include "atlas.hpp"  

Atlas::Atlas(int width, int height)  

    : ubo("mediasCoords", 100), width(width), height(height),rbp::GuillotineBinPack(width,height) {  }


Atlas::Atlas(std::string path, int width, int height) : Atlas(width,height) {

    init();

    texture.create(&data[0],width,height,0,0,1);

    texture.mipmaps = 10;

    TourKit::Directory dir(path);

    for (auto file:dir.list) { // add dir.list.sort.by bigger px count (w*h) first

        std::string filepath = dir.path_str+file;
        TourKit::Image img(filepath);

        if (!add(img.width,img.height,&img.data[0])) std::cout << "need new atlas for that" <<std::endl;
    
    }

}

void Atlas::link(ShaderProgram* shader) {

    // for (auto r:list) normalized_list.push_back({r.width/(float)width, r.height/(float)height, r.x/(float)width, r.y/(float)height});

    ubo.data.resize(list.size()*4*4);

    ubo.link(shader->id);

    ubo.update(&normalized_list[0], list.size()*4*4);

    shader->sendUniform("mediasAtlas", 1);

}

void Atlas::init() {

    buffered = true;

    data.resize(width*height*3); 

}
 
bool Atlas::add(int width, int height, unsigned char* data){ 

    for (auto cell : GetFreeRectangles() )  {

        if (width < cell.width && height < cell.height) {

            auto r = Insert(
                
                width, height,
                0,
                rbp::GuillotineBinPack::RectBestAreaFit, 
                rbp::GuillotineBinPack::SplitShorterAxis
            
            );

            normalized_list.push_back({r.width/(float)this->width, r.height/(float)this->height, r.x/(float)this->width, r.y/(float)this->height});
            
            if (buffered) texture.update(&data[0],r.width,r.height,r.x,r.y);

            return true;

        }

    }

    return false;

}