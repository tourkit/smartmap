#include "atlas.hpp"

#include "folder.hpp"
#include "engine.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "ubo.hpp"
#include "buffer.hpp"
#include "builder.hpp"
#include "struct.hpp"
#include "member.hpp"
#include "instance.hpp"
#include "utils.hpp"

#include "image.hpp"


Atlas::Atlas(int width, int height, std::string path)  : binpack(width,height,0), s("atlas"), effector(this) {

    static Struct& media_struct = Struct::create("Rect",0).add<glm::vec2>("size").add<glm::vec2>("pos");

    s.ref(&media_struct);

    engine.static_ubo.add(&s);

    medias = &Instance(&engine.static_ubo).find(&s).track();

    texture = new Texture(width,height,1,1);

    texture->mipmaps = 10;

    if (path.length()) 
        fromDir(path);

}

void Atlas::clear() {

    s.clear();

    binpack.Init(texture->width,texture->height);

}

void Atlas::fromDir(std::string path) {

    texture->clear();

    if (!Folder::exist(path)) {
        PLOGE << "atlas error"; return; 
    }

    this->path = path;

    clear();

    Folder dir(path, false);

    for (auto &file:dir.files) {

        Image img(file.path_v);

        if (!img.loaded) continue;

        auto r = binpack.Insert(img.width, img.height, rbp::MaxRectsBinPack::RectBestShortSideFit);
        if (!r.width) {PLOGE << img.name() << " can't fit, need more space."; continue;}

        float x[4] = {r.width/(float)this->texture->width, r.height/(float)this->texture->height, r.x/(float)this->texture->width, r.y/(float)this->texture->height};

        auto m = medias->push(&x[0]);

        texture->write(&img.data[0],r.width,r.height,r.x,r.y,1,1);

    }

    engine.static_ubo.upload();

}

void Atlas::link(ShaderProgram* shader) {

    shader->sendUniform(s.name(), 1);

    texture->bind();

}

///// EFFECTOR


// AtlasEffector  ////////////////


void Atlas::Effector::post(Builder* builder) { 



}

bool Atlas::Effector::setup(Builder* builder) { 

    builder->samplers[1] = s.name()+"_pass";

    ADD_UNIQUE<::Effector*>(builder->effectors_fragment, this);
    
    atlas->texture->bind();
    
    return true; 
    
}

std::string Atlas::Effector::source() {

    std::string out;

    if (!atlas || !s.size()) return out;

    out += "void "+s.name()+"(float id_) {\n\n";
    out +=     "\tint id = int(id_*"+std::to_string(atlas->medias->def()->size())+");\n\n";
    out +=     "\tvec2 tuv = uv;\n\n";
    out +=     "\ttuv *= static_ubo."+s.name()+"[id].size;\n";
    out +=     "\ttuv += static_ubo."+s.name()+"[id].pos;\n";
    out +=     "\tcolor *= texture("+s.name()+"_pass, tuv);\n\n";
    out += "}\n\n\n\n";

    return out;

}

Atlas::Effector::Effector(Atlas* atlas) : ::Effector(atlas->s.name()), atlas(atlas) {

    s.add<float>("id_").range(0, atlas->s.size(), 0);

}
