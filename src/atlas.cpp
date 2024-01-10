#include "atlas.hpp"  
#include "directory.hpp"  

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

void Atlas::clear() {

   
    // auto t = childrens;
    // for (auto c:t) { delete c; }

    childrens.resize(0);

    PLOGD << "clear";

    normalized_list.resize(0);

}

void Atlas::fromDir(std::string path) {

    if (!Directory::exist(path)) return;

    this->path = path;

    clear();

    Directory dir(path);

    for (auto file:dir.list) {
        
        auto img = new Image(path+"/"+file);

        if (!img->data.size()) { delete img; return;}
        
        add(img);
    
    }
}

void Atlas::editor() { 

    char path[512];

    memset(&path[0],0,512);

    if (ImGui::InputText("path", &path[0],512) && this->path != path) { 

    fromDir(path);
        
    }

}

void Atlas::link(ShaderProgram* shader) {

    ubo->subscribers.push_back(shader);
    ubo->update();

    ubo->upload();
    
    shader->sendUniform("mediasAtlas", 1);
    texture->bind();

}
 