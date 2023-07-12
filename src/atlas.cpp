#include "atlas.hpp"  

Atlas::Atlas(std::string path, int width, int height)  : binpack(width,height,0)  {

    texture = new Texture(nullptr,width,height,0,0,1);

    texture->mipmaps = 10;

    Directory dir(path);

    for (auto file:dir.list) { 

        Image img(file);

        auto r = binpack.Insert(img.width, img.height, rbp::MaxRectsBinPack::RectBestShortSideFit);

        if (!r.width) {std::cout << "needniouatlas" << std::endl; continue;}

        normalized_list.push_back({r.width/(float)this->texture->width, r.height/(float)this->texture->height, r.x/(float)this->texture->width, r.y/(float)this->texture->height});
        
        texture->write(&img.data[0],r.width,r.height,r.x,r.y);

    }

    ubo = new UBO("mediasCoords", 4*normalized_list.size());

}

void Atlas::link(ShaderProgram* shader) {

    ubo->link(shader->id);

    ubo->update(&normalized_list[0], normalized_list.size()*16); // 16 is size of Rect

    shader->sendUniform("mediasAtlas", 1);

}
 