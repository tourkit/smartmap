/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "engine.hpp"
#include "layer.hpp"
#include "texture.hpp"
#include "editor.hpp"
#include "node.hpp"

struct SmartLayer : Layer {

    int draw_count = 5;

    SmartLayer() : Layer(1920,1080) {  addModel(&VBO::quad);addModel(&VBO::quad); shader.builder = &builder; }


    void draw() override {

        if (feedback) { feedback->bind(); }

        fb.clear();

        shader.use();

        vbo.draw(draw_count);

        if (feedback) { return feedback->read(fb.texture); }

    }

        struct ShaderProgramBuilder : DrawCall::ShaderProgramBuilder {

        // void build() override;
        void frag() {

            body_fragment+= "\tif (ID == 0) {\n\n";
            body_fragment+= "\t//\tCOLOR = texture(render_pass,NORMALIZED)-.02;//*static_ubo.ubervbo[int(OBJ)].uberLayer.size+static_ubo.ubervbo[int(OBJ)].uberLayer.norm)-.02;\n\n";
            body_fragment+= "\t\treturn;\n\n";
            body_fragment+= "\t}\n\n";
            body_fragment+= "\tif (ID > 1) return;\n\n";
            body_fragment+= "\tCOLOR = vec4(UV.x+.01);\n\n";
            body_fragment+= "\tif (OBJ==0) { COLOR *= vec4(0,1,.1,1); }\n\n";
            body_fragment+= "\telse if (OBJ==1) { COLOR *= vec4(0,0,1,1); }\n\n";
            body_fragment+= "\telse if (OBJ==2) { COLOR *= vec4(1,0,1,1); }\n\n";
            body_fragment+= "\telse if (OBJ==3) { COLOR *= vec4(1,0,0,1); }\n\n";
            body_fragment+= "\telse if (OBJ==4) { COLOR *= vec4(1,1,0,1); }\n\n";
            body_fragment+= "\telse if (OBJ==5) { COLOR *= vec4(0,1,0,1); }\n\n";
            body_fragment+= "\telse { \n\n";
            body_fragment+= "\t\tif (OBJ>1) COLOR *= .5;\n\n";
            body_fragment+= "\t}\n\n";
            body_fragment+= "\tif ( NORMALIZED.x < 0 || NORMALIZED.y < 0 || NORMALIZED.x > 1 || NORMALIZED.y > 1 ) COLOR = vec4(0);\n\n";

        }

        void vert() {

            body_vertex += "\tPOS = NORMALIZED;\n\n";
            body_vertex += "\tif (ID == 1) {\n\n";
            body_vertex += "\t\tUV = POS;\n\n";
            body_vertex += "\t\tint curr = dynamic_ubo[0].eNGINE.alt;\n\n";
            body_vertex += "\t\tvec2 size = dynamic_ubo[curr].uberLayer1.smartLayer1[int(0)].rectangle.size;\n\n";
            body_vertex += "\t\tvec2 pos = dynamic_ubo[curr].uberLayer1.smartLayer1[int(0)].rectangle.pos;\n\n";
            body_vertex += "\t\tpos.x = mod(dynamic_ubo[curr].eNGINE.frame,100)/100;\n\n";
            body_vertex += "\t\tPOS *= size;\n\n";
            body_vertex += "\t\tPOS += pos*(1+size);//-1;\n\n";
            body_vertex += "\t\tPOS -= size;\n\n";
            body_vertex += "\t}\n\n";
            body_vertex += "\tNORMALIZED = POS;\n\n";
            body_vertex += "\t// POS *= static_ubo.ubervbo[int(OBJ)].uberLayer.size;\n\n";
            body_vertex += "\t// POS += static_ubo.ubervbo[int(OBJ)].uberLayer.norm;\n\n";
            body_vertex += "\tPOS = POS*2-1;\n\n";
        }


    } builder;

};


int main() {

Editor<SmartLayer>([](Node* node, SmartLayer* layer) { Editor<Layer>::cb(node, layer); });
NODE<SmartLayer>::onrun( [](Node* node, SmartLayer* layer) { NODE<Layer>::onrun_cb(node, layer); });
NODE<SmartLayer>::onchange( [](Node* node, SmartLayer* layer) { NODE<Layer>::onchange_cb(node, layer); });

    engine.init();


    engine.open("project.json");

    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);

    auto sl_ = engine.stack->addOwnr<SmartLayer>()->select();
    auto &sl = *sl_->get();
    if (dynamic_cast<Layer*>(&sl) != nullptr) std::cout  <<"OOOOOOOOOOOOOOOOO";
    if (typeid(*sl_->ptr) == typeid(Layer)) { std::cout << "SISIS" << std::endl; }

    sl.shader.create();

    engine.run();

}

// fix struct deletion and remap

// Layer::dyninst && Layer::statinst (also for editors ;) though need per Effectable ... oulalal)

// cpu transform matrice matrice_elem_XY

// fine feedback ~ish

// freetype

// artnet output

// later gater :

// deletion is fucked

// static Effector should  update on file update

// c# librehardwaremonitor get temps &set fanspeed@burne

// npcap


// EDID SELECT HAPPEN ON RESTART ONLY

// EXTRA CONTROLER CRASH - NO TURN OFF ON RACK HARD KILL THEN WRONG PACKE T// need auto restart on crash (aka WRONG PACKET)

// SIGNALKILL/EDIDMAN NOT WORK W/OUT EXTRA CONTROLER // more linked to mainboard ?

// CODING WHEEL REALLY SHITY // NOT INTUITIVE MISLEADING , NEED 4 click button wheel OR 4 button not both

// MOVE EDID MENU PER OUTPUT IN OUTPUT->X->EDID

// DISPLAY INFO MESSAGE ON START / SHUTDOWN  "STARTING" "SHUTTING DOWN"

// NETWORK REPEAT FAILURE
