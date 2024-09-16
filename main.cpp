

/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */


// many vlayers

// delete vlayer should remove corresponding models

// wrapper in layer def

// feedback in layer def

// freetype

// grid / quantity 

// ----


#include "boilerplate.hpp"
#include "engine.hpp"
#include "builder.hpp"
#include "layer.hpp"
#include "gui.hpp"


// fix dynubo && statubo ..

// fix Member::adding all occurence set default  ..




int main() {

    // Boilerplate::Init();

    logger.cout(Sev::warning);

    engine.init();

    engine.open("project.json");

    auto lay = engine.stack->childrens[0]->is_a<Layer>();

    auto &builder = DrawCall::builder;

    builder.dc = lay;
    builder.build();

    BoilerUBO bubo;
    BoilerGUI bgui;

    builder.header_fragment += "\n\nlayout (binding = 2, std140) uniform uboss { vec4 xx; };\n";
    builder.body_fragment = "\tCOLOR = vec4(xx.x);\n";
    builder.body_fragment += "\t // COLOR = vec4(dynamic_ubo[0].free_Layer_1.quad1[0].argb.alpha);\n";
    builder.body_fragment += "\t // COLOR = vec4(static_ubo.layers.dim.x);\n";

    lay->shader.create(builder.frag(), builder.vert());

    bubo.bind(lay->shader.id,2);

    for (auto ubo : builder.ubos) 
        ubo->bind(lay->shader.id);

    int sampler_id = 0;
    for (auto s : builder.samplers) {
    
        if (s.first > sampler_id) {
            // for ( int i = sampler_id; i < s.first; i++) program.sendUniform("randi"+std::to_string(i), i);
            sampler_id = s.first;
        }

        s.second->bind(s.first);
        lay->shader.sendUniform(s.second->sampler_name, s.first);
        
        sampler_id++;
    }

    struct XXX : GUI::Window {

        using GUI::Window::Window;

        float* ptr;

        void draw() override {
            
            for (int i = 0; i < 4; i++) 
                ImGui::SliderFloat(("f"+std::to_string(i)).c_str(), ptr+i,0,1);
            
        }
    }
     xxxsa("test",engine.gui_v);
     xxxsa.ptr = &bubo.data[0];

    engine.run([&](){bubo.upload();});




    // Member buff("buff");
    // buff.buffering(true);

    // Member layer("layer");
    // layer.quantity(3);
    // buff.add(&layer);

    // Member quad("quad");
    // quad.quantity(2); 
    // layer.add(&quad);

    // Member argb("argb");
    // argb.add<char>("ch").range(0,1,2); 
    // Member otro("otro");
    // otro.add<float>("ch").range(0,1,3); 

    // Member ref("ref");
    // ref.add(&argb);
    // quad.add(&ref);

    // Member otroref("otroref");
    // otroref.add(&otro);
    // quad.add(&otroref);

    // Instance inst(buff);

    // Instance refinst(inst);
    // refinst[&layer][&quad][&ref];//.parent();


    // inst.print();

}
