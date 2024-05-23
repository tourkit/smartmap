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
#include "vbo.hpp"


#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <cstdint>

#include "RectangleBinPack/MaxRectsBinPack.h"

struct UberLayer {

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

    UberLayer() : layers_def("Layers") {

        layers_def.add<glm::vec2>("size");
        layers_def.add<glm::vec2>("pos");
        layers_def.quantity(0);
        engine.static_ubo.add(&layers_def);

        // glsl_struct = &engine.static_ubo[layers_def.name()].track();

        // glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);

        // PLOGD << "max tex size : " << max_tex_size;

    }

    void calc(VBO* vbo) {

        auto layers = this->layers;

        std::sort( layers.begin(), layers.end(), [](auto a,auto b){ return a.h > b.h; } );

        auto first_of_line = layers.begin();

        int max_line_h = 0;
        int last_x = 0;
        int last_y = 0;

        std::vector<std::vector<std::array<int,5>>> matrice = {{}};

        for (auto it = layers.begin(); it != layers.end(); it++ ) {

            if (it->h > max_line_h) max_line_h = it->h;

            if (last_x+it->w > max_tex_size/2) { // beware /2 just for me

                matrice.resize( matrice.size()+1 );
                last_y += max_line_h;
                max_line_h = 0;
                last_x = 0;

            }else{

                matrice.back().emplace_back( std::array<int,5>{it->w, it->h, last_x, last_y, it->id} );
                last_x += it->w;

            }



        }

        if (!matrice.size() || !matrice.back().size()) return;

        float matrice_height = matrice.back()[0][1]+matrice.back()[0][3];
        float matrice_width = matrice[0].back()[0]+matrice[0].back()[2];

        std::vector<std::array<float,4>> normalized;

        normalized.resize(layers.size());

        glsl_struct->def()->quantity(layers.size());

        for (auto &x : matrice) for (auto &y : x) glsl_struct->eq(y[4]).set<glm::vec4>(glm::vec4(y[0] / matrice_width,y[1] / matrice_height,y[2] / matrice_width,y[3] / matrice_height));

    }

    void add(int w , int h) { // kinda ctor for VLaye

        layers.emplace_back(w,h,layers.size());

     }

};


int main() {

    engine.init();

    engine.render_passes.emplace_back(1920*5,1080*5);
    engine.debug->addPtr<FrameBuffer>(&engine.render_passes.back());

    UberLayer ubl;

    // Struct test("test");
    // test.add<glm::vec2>("size");
    // test.add<glm::vec2>("pos");
    // test.quantity(0);
    // engine.static_ubo.add(&test);
    // engine.static_ubo.upload();

    // ubl.add(192,108);
    // ubl.add(192,108);
    // ubl.add(192,108);


    // ubl.calc(nullptr);


    engine.open("project.json");


    engine.run();

}

// RE QUANTITY not atlas work duh

// matrice pass

// linuxify a.k.a gl1.1 is the pb

// artnet output

// fine feedback

// later gater :

// ~save remap

// static Effector should  update on file update

// sort && thus add fx & models accordingly

// timelines (thus player) maybeee

// model editor about instance :) c ( engine.selected should be instance? inst is only members.)

// drop remap
