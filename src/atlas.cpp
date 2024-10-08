#include "atlas.hpp"

#include "file.hpp"
#include "folder.hpp"
#include "engine.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "ubo.hpp"
#include "buffer.hpp"
#include "builder.hpp"
#include "instance.hpp"
#include "utils.hpp"

#include "image.hpp"
#include <array>


Atlas::Atlas(int width, int height, std::string path)  : binpack(width,height,false), m("atlas"), effector(this) {

    m.striding(true);
    
    m.quantity(0);
    
    m.add(&globals.rect);

    engine.static_ubo->add(&m);

    medias = &(*new Instance(*engine.static_ubo))[&m];

    texture = new Texture(width,height,1,1);

    texture->mipmaps = 10;

    if (path.length()) 
        fromDir(path);

}

void Atlas::clear() {

    m.quantity(0);

    binpack.Init(texture->width,texture->height, false);

}

void Atlas::fromDir(std::string path) {

    texture->clear();

    if (!Folder::exist(path)) {
        PLOGE << "atlas error"; return; 
    }

    this->path = path;

    clear();

    Folder dir(path, false);

    std::map<File*, std::array<float, 4>> temp_list;

    for (auto file:dir.files) {

        Image img(file->path_v);

        if (!img.loaded) continue;

        auto r = binpack.Insert(img.width, img.height, rbp::MaxRectsBinPack::RectBestShortSideFit);
        if (!r.width) 
            {PLOGE << img.name() << " can't fit, need more space."; continue;}

        temp_list[file] = {r.width/(float)this->texture->width, r.height/(float)this->texture->height, r.x/(float)this->texture->width, r.y/(float)this->texture->height};

        texture->write(&img.data[0],r.width,r.height,r.x,r.y);

    }

    for (auto x : dir.files) {

        m.quantity(m.quantity()+1);

        medias->eq(m.quantity()-1).set<std::array<float,4>>(temp_list[x]);

    }

    engine.static_ubo->upload();

}

void Atlas::link(ShaderProgram* shader) {

    shader->sendUniform(m.name(), 1);

    texture->bind();

}

///// EFFECTOR


// AtlasEffector  ////////////////


bool Atlas::Effector::setup(Builder* builder) { 

    builder->addSampler(atlas->texture, name());

    ADD_UNIQUE<::Effector*>(builder->effectors_fragment, this);
    
    return true; 
    
}

std::string Atlas::Effector::header() {

    std::string out;

    if (!atlas || !size()) return out;


    out += "void "+name()+"(float id_) {\n\n";
    out +=     "\tint id = int(id_*"+std::to_string(atlas->medias->stl.back().m->size())+");\n\n";
    out +=     "\tvec2 tuv = uv;\n\n";
    out +=     "\ttuv *= static_ubo."+name()+"[id].rect.size;\n";
    out +=     "\ttuv += static_ubo."+name()+"[id].rect.pos;\n";
    out +=     "\tcolor *= texture("+name()+"_pass, tuv);\n\n";
    out += "}\n\n\n\n";
    
    return out;

}

Atlas::Effector::Effector(Atlas* atlas) : ::Effector(atlas->m.name()), atlas(atlas) {

    add<float>("id_").range(0, 1, 0);

}
