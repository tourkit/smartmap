#include "atlas.hpp"  
#include "file.hpp"  

Atlas::Atlas(int width, int height)  

    : width(width), buffer(width,height, GL_RGB8), height(height),rbp::GuillotineBinPack(width,height) {  }


Atlas::Atlas(std::string path, int width, int height) : Atlas(width,height) {

    init();

    buffer.mipmaps = 10;

    TourKit::Directory dir(path);

    for (auto file:dir.list) {

        TourKit::Image img;
        img.loadflipped(dir.path_str+file);

        if (!add(img.width,img.height,&img.data[0])) std::cout << "need new atlas for that" <<std::endl;
    
    }

}

void Atlas::link(GL::ShaderProgram& shader) {

    normalize();

    atlaspos.create((normalized.size()-1)*4);
    atlaspos.link(shader, "mediasCoords");
    atlaspos.send((float*)&normalized[0].x,(normalized.size()-1)*4*4);
    
    shader.sendUniform("mediasAtlas", 1);
    buffer.bind(1);

}

void Atlas::init() {

    buffered = true;

    data.resize(width*height*3); 

}

void Atlas::normalize() {

    normalized.resize(0);

    for (auto r:list) normalized.push_back(vec4{r.width/(float)width, r.height/(float)height, r.x/(float)width, r.y/(float)height});
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
            
            if (buffered) buffer.upload(&data[0],r.width,r.height,r.x,r.y);

            return true;

        }

    }

    return false;

}