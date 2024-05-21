/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "framebuffer.hpp"
#include "vbo.hpp"
#include "shader.hpp"
#include "drawcall.hpp"
#include "instance.hpp"
#include "texture.hpp"


#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cstdint>

#include "RectangleBinPack/MaxRectsBinPack.h"

struct ChoumMatrice {

    Struct layers_def;

    Instance* glsl_struct;

    struct VLayer {

        int w ;
        int h;
        int id = 0;
    };

    FrameBuffer fb;

    std::vector<VLayer> layers;

    int w ; int h;

    GLint max_tex_size;

    ChoumMatrice() : layers_def("layers_def") {

        layers_def.add( &Engine::rect );

        engine.static_ubo.add(&layers_def);

        glsl_struct = &engine.static_ubo[layers_def.name()].track();

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);

        PLOGD << "max tex size : " << max_tex_size;

    }

    void calc() {

        auto layers = this->layers;

        std::sort( layers.begin(), layers.end(), [](auto a,auto b){ return a.h > b.h; } );

        auto first_of_line = layers.begin();

        int max_line_h = 0;
        int last_x = 0;
        int last_y = 0;

        std::vector<std::vector<std::array<int,5>>> matrice = {{}};

        for (auto it = layers.begin(); it != layers.end();) {

            if (it->h > max_line_h) max_line_h = it->h;

            if (last_x+it->w > max_tex_size/2) { // beware /2 just for me

                matrice.resize( matrice.size()+1 );
                last_y += max_line_h;
                max_line_h = 0;
                last_x = 0;

            }else{

                matrice.back().emplace_back( std::array<int,5>{it->w, it->h, last_x, last_y, it->id} );
                last_x += it->w;
                it++;

            }


        }

        float matrice_height = matrice.back()[0][1]+matrice.back()[0][3];
        float matrice_width = matrice[0].back()[0]+matrice[0].back()[2];

        std::vector<std::array<float,4>> normalized;

        normalized.resize(layers.size());
        glsl_struct->def()->quantity(layers.size());

        for (auto &x : matrice) {

            for (auto &y : x) {

                glsl_struct->eq(y[4]).set<glm::vec4>(glm::vec4(y[0] / matrice_width,y[1] / matrice_height,y[2] / matrice_width,y[3] / matrice_height));


                normalized[y[4]][0] = y[0] / matrice_width;
                normalized[y[4]][1] = y[1] / matrice_height;
                normalized[y[4]][2] = y[2] / matrice_width;
                normalized[y[4]][3] = y[3] / matrice_height;

            }

        }


    }

    void add(int w , int h) { // kinda ctor for VLaye

        layers.emplace_back(w,h,layers.size());




        calc();

     }

};


int main() {

    engine.init();

    engine.render_passes.emplace_back(1920*5,1080*5);
    engine.debug->addPtr<FrameBuffer>(&engine.render_passes.back());

    engine.open("project.json");

    // ChoumMatrice chm;

    // chm.add(1920,1080);
    // chm.add(1920,1081);
    // chm.add(1920,1060);
    // chm.add(1920,1080);
    // chm.add(1920,1080);
    // chm.add(1920,1080);
    // chm.add(1920,1080);
    // chm.add(1920,1080);
    // chm.add(1920,1080);
    // chm.add(1920,1080);
    // chm.add(1920,1080);
    // chm.add(1920,1080);

    // // chm.calc();


    // auto &vbo = engine.stack->childrens[2]->is_a<Layer>()->vbo;

    // PLOGD << vbo[0].eq(2)[0][1].get<glm::vec2>().y;

    // for (int i = 0 ; i < 4; i++) {

    //     auto inst = vbo[0].eq(i)[0][0];

    //    inst.set<glm::vec2>( inst.get<glm::vec2>() * glm::vec2(.1) );

    // }
    // vbo.upload();

    engine.run();

}

// matrice pass

// linuxify a.k.a gl1.1 is the pb

// artnet output

// fine feedback

// later gater :

// ~save remap

// static Effector should  update on file update

// RE QUANTITY not atlas work duh

// sort && thus add fx & models accordingly

// timelines (thus player) maybeee

// model editor about instance :) c ( engine.selected should be instance? inst is only members.)

// drop remap
