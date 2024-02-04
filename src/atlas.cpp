#include "atlas.hpp"  

#include "directory.hpp"  
#include "engine.hpp"  
#include "shader.hpp"  
#include "texture.hpp"
#include "ubo.hpp"


Atlas::Atlas(int width, int height, std::string path)  : binpack(width,height,0)  {

    texture = new Texture(width,height,1,1);

    texture->mipmaps = 10;

    if(path.length()) fromDir(path);

    // for (auto file:dir.list) { 

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

    // childrens.resize(0); // if enough remove above lines

    PLOGD << "clear";

    normalized_list.resize(0);

    binpack.Init(texture->width,texture->height);

}

void Atlas::fromDir(std::string path) {

    texture->clear();

    if (!Directory::exist(path)) return;

    this->path = path;

    clear();

    Directory dir(path);

    for (auto &file:dir.list) {

        PLOGD << file;

    }
    for (auto &file:dir.list) {if (((Image*)&file)->is_image()) { }}
    return;

    // for (auto c : childrens) {

    //     auto img = ((ImagePtr*)c)->ptr;

    //     auto r = binpack.Insert(img->width, img->height, rbp::MaxRectsBinPack::RectBestShortSideFit);
    //     if (!r.width) {PLOGW << "needniouatlas"; continue;} 

    //     normalized_list.push_back({r.width/(float)this->texture->width, r.height/(float)this->texture->height, r.x/(float)this->texture->width, r.y/(float)this->texture->height});

    //     texture->write(&img->data[0],r.width,r.height,r.x,r.y,1,1);

    // }
}

// void Atlas::editor() { 

//     char path[512];

//     memset(&path[0],0,512);

//     if (ImGui::InputText("path", &path[0],512) && this->path != path) fromDir(path);

//     float ratio = texture->height/(float)texture->width;
//     auto nw = std::min(texture->width,(GLuint)512);
//     ImGui::Image((void*)(intptr_t)(ImTextureID)(uintptr_t)texture->id,  ImVec2(nw,nw*ratio));

//     ImGui::SameLine();;

//     auto selected = Engine::getInstance().editorw.selected;
    
//     if (selected && typeid(*selected) == typeid(ImagePtr)) ImGui::InputInt2(selected->name.c_str(), &((ImagePtr*)selected)->ptr->width);

// }

void Atlas::link(ShaderProgram* shader) {

    ubo->subscribers.push_back(shader);
    ubo->update();

    ubo->upload();
    
    shader->sendUniform("mediasAtlas", 1);
    texture->bind();

}
 