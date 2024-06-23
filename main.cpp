/*

   SMARTMAP ( v 1.3.1.2 beta )

                                */

#include "engine.hpp"
#include "layer.hpp"
#include "texture.hpp"
#include "editor.hpp"
#include "model.hpp"
#include "node.hpp"

#include "vendors/imgui/imgui_internal.h"

struct SmartLayer : Layer {

    int draw_count = 5;

    SmartLayer() : Layer(1920,1080) {  addModel(&VBO::quad); shader.builder = &builder; }


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
            body_fragment+= "\t\tCOLOR = texture(render_pass,NORMALIZED)-.02;//*static_ubo.ubervbo[int(OBJ)].uberLayer.size+static_ubo.ubervbo[int(OBJ)].uberLayer.norm)-.02;\n\n";
            body_fragment+= "\t\treturn;\n\n";
            body_fragment+= "\t}\n\n";
            body_fragment+= "\tCOLOR = vec4(1);\n\n";
            body_fragment+= "\tif ( NORMALIZED.x < 0 || NORMALIZED.y < 0 || NORMALIZED.x > 1 || NORMALIZED.y > 1 ) COLOR = vec4(0);\n\n";

        }

        void vert() {

            body_vertex += "\tPOS = NORMALIZED;\n\n";

            body_vertex += "\tint id = int(mod(ID,8));\n\n";
            body_vertex += "\tif (id >0) {\n\n";
            body_vertex += "\t\tOBJ = (id-1)/8;\n\n";
            body_vertex += "\t\tint last = dynamic_ubo[0].eNGINE.alt;\n";
            body_vertex += "\t\tint curr = dynamic_ubo[1].eNGINE.alt;\n\n";
            body_vertex += "\t\tvec2 size = dynamic_ubo[curr].uberLayer1.smartLayer1[int(OBJ)].rectangle.size;\n";
            body_vertex += "\t\tvec2 pos = dynamic_ubo[curr].uberLayer1.smartLayer1[int(OBJ)].rectangle.pos;\n\n";
            body_vertex += "\t\tfloat steps = 6;\n\n";
            body_vertex += "\t\tfloat smoothing = .8;\n\n";
            body_vertex += "\t\tvec2 pos2 = dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.pos;\n\n";
            body_vertex += "\t\tfloat step = (int(ID)-1)/steps;\n\n";
            body_vertex += "\t\tif (abs(size.x-dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.size.x)<.015 && abs(size.y-dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.size.y)<.015) size = mix(size,dynamic_ubo[last].uberLayer1.smartLayer1[int(OBJ)].rectangle.size,step);\n\n";
            body_vertex += "\t\tif (abs(pos.x-pos2.x)<.12 && abs(pos.y-pos2.y)<.12) pos = mix(pos,pos2,step);\n\n";
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

void struct_editor(Node* node, Member* m, int offset = 0) {

    static std::vector<std::shared_ptr<Struct>> owned_structs;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX()+offset*20);

                    // ImGui::PushID(*(int*)m);
    ImGui::Text( ("+"+m->name()).c_str() );

    if (ImGui::BeginPopupContextItem(m->name().c_str())) {

        // ImGui::Text(m->name().c_str());
        ImGui::Separator();
        if (ImGui::MenuItem("new struct")) {  node->addOwnr<Struct>(); }
        ImGui::Separator();
        if (ImGui::MenuItem("new data")) {}
        ImGui::Separator();

        ImGui::EndPopup();
    }

                    // ImGui::PopID();
    offset += 1;

    for (auto x : m->members) struct_editor( node, x, offset );



}


int main() {

Editor<Struct>([](Node* node, Struct* s){ struct_editor(node,s); });

Editor<SmartLayer>([](Node* node, SmartLayer* layer) {


    if (ImGui::BeginTabBar("damdmamd")) {

        if (ImGui::BeginTabItem("layer")) {

            ImGui::SliderInt("draw count", &layer->draw_count, 0 , 20);

            Editor<Layer>::cb(node, layer);

            ImGui::EndTabItem();

        }

        if (ImGui::BeginTabItem("specials")) {

            ImGui::SliderInt("draw count", &layer->draw_count, 0 , 20);
            Editor<Struct>::cb(node, &layer->s);

            ImGui::EndTabItem();

        }

        ImGui::EndTabBar();
    }


});



NODE<SmartLayer>::onrun( [](Node* node, SmartLayer* layer) { NODE<Layer>::onrun_cb(node, layer); });
NODE<SmartLayer>::onchange( [](Node* node, SmartLayer* layer) { NODE<Layer>::onchange_cb(node, layer); });

    engine.init();


    engine.open("project.json");

    engine.gui->delete_list.push_back(engine.stack->childrens[0]);
    // delete engine.stack->childrens[0];


    // glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_COLOR);

    // auto sl_ = engine.stack->addOwnr<SmartLayer>()->select();
    // auto &sl = *sl_->get();
    // engine.stack->addPtr<Struct>(&sl.s);
    // if (dynamic_cast<Layer*>(&sl) != nullptr) std::cout  <<"OOOOOOOOOOOOOOOOO";
    // if (typeid(*sl_->ptr) == typeid(Layer)) { std::cout << "SISIS" << std::endl; }

    // sl.shader.create();

    engine.run();

}


// Modelable editor  Modelable::inst() isntance UI dude; + D&D Node::s* in remap

// check  isnt deletion mgmt plz

// WrapperEffector : Effector

// fine feedback ~ish

// freetype

// artnet output



// test 1+nth_pass*objcopunt inst,

// test 2 OBJ  10 inst, PER / INST // with 1 first nan ?>


// cpu transform matrice matrice_elem_XY



// later gater :

// static Effector should  update on file update

// c# librehardwaremonitor tryiing // theb ImGraphView get temps &set fanspeed@burne

// npcap



// EDID SELECT HAPPEN ON RESTART ONLY

// EXTRA CONTROLER CRASH - NO TURN OFF ON RACK HARD KILL THEN WRONG PACKE T// need auto restart on crash (aka WRONG PACKET)

// SIGNALKILL/EDIDMAN NOT WORK W/OUT EXTRA CONTROLER // more linked to mainboard ?

// CODING WHEEL REALLY SHITY // NOT INTUITIVE MISLEADING , NEED 4 click button wheel OR 4 button not both

// MOVE EDID MENU PER OUTPUT IN OUTPUT->X->EDID

// DISPLAY INFO MESSAGE ON START / SHUTDOWN  "STARTING" "SHUTTING DOWN"

// NETWORK REPEAT FAILURE
