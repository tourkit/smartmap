#include "atlas.hpp"

#include "folder.hpp"
#include "engine.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "ubo.hpp"
#include "buffer.hpp"
#include "struct.hpp"
#include "member.hpp"
#include "instance.hpp"

#include "image.hpp"
#include "log.hpp"


Atlas::Atlas(int width, int height, std::string path)  : binpack(width,height,0)  {

    static Struct& media_struct = Struct::create("Media",0).add<glm::vec2>("size").add<glm::vec2>("pos");

    engine.static_ubo.add(&media_struct);

    buffer = &media_struct;

    texture = new Texture(width,height,1,1);

    texture->mipmaps = 10;

    if (path.length()) fromDir(path);

}

void Atlas::clear() {

    PLOGI << "TODO buffer clear()";

    binpack.Init(texture->width,texture->height);

}

void Atlas::fromDir(std::string path) {

    texture->clear();

    if (!Folder::exist(path)) return;

    this->path = path;

    clear();

    Folder dir(path, false);

    for (auto &file:dir.files) {

        Image img(file.path);

        if (!img.loaded) continue;

        auto r = binpack.Insert(img.width, img.height, rbp::MaxRectsBinPack::RectBestShortSideFit);
        if (!r.width) {PLOGE << img.name() << " can't fit, need more space."; continue;}

        float x[4] = {r.width/(float)this->texture->width, r.height/(float)this->texture->height, r.x/(float)this->texture->width, r.y/(float)this->texture->height};

        auto m = engine.static_ubo["Media"].push(&x[0]);

        texture->write(&img.data[0],r.width,r.height,r.x,r.y,1,1);

    }

    engine.static_ubo.upload();

}

void Atlas::link(ShaderProgram* shader) {

    shader->sendUniform("medias", 1);

    texture->bind();

}
