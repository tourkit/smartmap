#include "atlas.hpp"  

Atlas::Atlas(int width, int height)  

    : atlaspos("mediasCoords", 100), width(width), height(height),rbp::GuillotineBinPack(width,height) {  }


Atlas::Atlas(std::string path, int width, int height) : Atlas(width,height) {

    init();

    texture.create(&data[0],width,height);

    texture.mipmaps = 10;

    TourKit::Directory dir(path);

    for (auto file:dir.list) {

        std::string filepath = dir.path_str+file;
        TourKit::Image img(filepath);

        if (!add(img.width,img.height,&img.data[0])) std::cout << "need new atlas for that" <<std::endl;
    
    }

}

void Atlas::link(ShaderProgram* shader) {

    std::vector<std::array<float,4>> normalized;

    for (auto r:list) {
    
        normalized.push_back({r.width/(float)width, r.height/(float)height, r.x/(float)width, r.y/(float)height});
        std::cout << r.width/(float)width << " " << r.height/(float)height << " " << r.x/(float)width << " " << r.y/(float)height << std::endl;
    
    }

    for (int i = 0; i < 10; i++) { std::cout << (GLuint)data[i] << " "; }
    std::cout << std::endl;


    atlaspos.data.resize(list.size()*4*4);

    atlaspos.link(shader->id);

    atlaspos.update(&normalized[0], list.size()*4*4);

    shader->sendUniform("mediasAtlas", 1);
    texture.bind(1);

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
            
            if (buffered) texture.update(&data[0],r.width,r.height,r.x,r.y);

            return true;

        }

    }

    return false;

}