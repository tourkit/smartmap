#include "atlas.hpp"  

Atlas::Atlas(int width, int height, std::string path)  : Node("Atlas"), binpack(width,height,0)  {

    texture = new Texture(width,height,1,1);

    texture->mipmaps = 10;

    if(path.length()) fromDir(path);

    // for (auto file:dir.list) { 

    //     Image img(file);

    //     add(new Node( img.name + " " + std::to_string(img.width) + "x" + std::to_string(img.height))); // should create a ImageRef then use <ImageRef> to binpack ..

    //     auto r = binpack.Insert(img.width, img.height, rbp::MaxRectsBinPack::RectBestShortSideFit);

    //     if (!r.width) {PLOGW << "needniouatlas"; continue;} 


    //     normalized_list.push_back({r.width/(float)this->texture->width, r.height/(float)this->texture->height, r.x/(float)this->texture->width, r.y/(float)this->texture->height});

    //     texture->write(&img.data[0],r.width,r.height,r.x,r.y,1,1);

    // }

    // ubo = new UBO("mediasCoords");
    // ubo->buffer.add("mediasCoords", {

    //     "Size",
    //     "Position",

    // }, normalized_list.size());

    // memcpy(&ubo->buffer.data[0],&normalized_list[0],normalized_list.size()*16); // 16 is size of Rect

}

void Atlas::fromDir(std::string path) {

    Directory dir(path);

    // for (auto file:dir.list) add(new Image(file));

}

void Atlas::link(ShaderProgram* shader) {

    ubo->subscribers.push_back(shader);
    ubo->update();

    ubo->upload();
    
    shader->sendUniform("mediasAtlas", 1);
    texture->bind();

}
 