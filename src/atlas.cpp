#include "atlas.hpp"  

#include "directory.hpp"  
#include "engine.hpp"  
#include "shader.hpp"  
#include "texture.hpp"
#include "ubo.hpp"
#include "object.hpp"
#include "struct.hpp"

#include "image.hpp"
#include "log.hpp"


Atlas::Atlas(int width, int height, std::string path)  : binpack(width,height,0)  {

    buffer = engine.static_ubo->addObj(new Struct("Media", {"size","pos"}));

    texture = new Texture(width,height,1,1);

    texture->mipmaps = 10;

    if(path.length()) fromDir(path);

}

void Atlas::clear() {

    PLOGI << "TODO buffer clear()";

    binpack.Init(texture->width,texture->height);

}

void Atlas::fromDir(std::string path) {

    texture->clear();

    if (!Directory::exist(path)) return;

    this->path = path;

    clear();

    Directory dir(path);

    for (auto &file:dir.list) {
        
        Image img(file);

        if (!img.loaded) continue;
        
        auto r = binpack.Insert(img.width, img.height, rbp::MaxRectsBinPack::RectBestShortSideFit);
        if (!r.width) {PLOGW << img.name << " can't fit, need more space."; continue;} 

        float x[4] = {r.width/(float)this->texture->width, r.height/(float)this->texture->height, r.x/(float)this->texture->width, r.y/(float)this->texture->height};

        buffer->push(&x[0]);

        texture->write(&img.data[0],r.width,r.height,r.x,r.y,1,1);

    }

}

void Atlas::link(ShaderProgram* shader) {

    engine.static_ubo->subscribers.push_back(shader);
    engine.static_ubo->update();

    engine.static_ubo->upload();
    
    shader->sendUniform("medias", 1);
    texture->bind();

}
 