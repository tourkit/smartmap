#include "callbacks.hpp"

#include "node.hpp"
#include "file.hpp"
#include "ubo.hpp"
#include "struct.hpp"
#include "model.hpp"
#include "effector.hpp"
#include "folder.hpp"
#include "drawcall.hpp"
#include "engine.hpp"
#include "artnet.hpp"
#include "ndi.hpp"
#include "atlas.hpp"
#include "json.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "layer.hpp"
#include "texture.hpp"

void Callbacks::init() {

    // ////////// xxx.HPP

    // NODE<xxx>::oncreate([](Node* node, xxx *x){ });
    // NODE<xxx>::onchange([](Node* node, xxx *x){ });
    // NODE<xxx>::onrun([](Node* node, xxx *x){ });

    // NODE<xxx>::onadd<yyy>([](Node*_this,Node*node){

    //     auto x = _this->is_a<xxx>();
    //     auto y = node->is_a<yyy>();

    //     return _this->Node::add(node);

    // });
    
    ////////// FILE.HPP

    NODE<File>::oncreate([](Node* node, File *file){ node->name(file->name()); });

    NODE<File>::onrun([](Node* node, File *file){

        // if (file->last_modified != file->getTimeModified()) file->reload();
        if (file->hasChanged()) {
            file->reload();
            node->update();


            }

    });

    ////////// Struct.HPP

    NODE<Struct>::oncreate([](Node* node, Struct *s){ 
        node->name(s->name()); 
    });

    NODE<Struct>::onchange([&](Node*node, Struct* s){
        if (s->name() != node->name())
            s->name(node->name()); });



    ////////// ENGINE

    NODE<UBO>::onrun([](Node* node, UBO *ubo){ ubo->upload(); });

    NODE<UBO>::oncreate([](Node* node, UBO *ubo){ node->name(ubo->name()); });


    ////////// DRAWCALL

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){

        auto x = _this->addOwnr<Layer>();
        x->add(node)->active(false);
        return x;

    });
    
    NODE<Stack>::onchange([](Node*node, Stack* stack){

        for (auto c : node->childrens) {

            Layer* layer = c->is_a<Layer>();

            if (layer) layer->shader.create();
        }
        
    });

    NODE<Layer>::ondelete([](Node* node, Layer *layer){ 

        for (auto x : node->referings) {

            auto o = x->is_a<Output>();

            if (o && o->fb == &layer->fb) o->fb = nullptr;
 
        } 
        
    }); 

    NODE<Model>::ondelete([](Node* node, Model *model) {

        auto dc = node->parent()->is_a_nowarning<DrawCall>();
        if (dc) { dc->removeModel(model); return; }

        auto layer = node->parent()->is_a_nowarning<Layer>();
        if (layer) { layer->removeModel(model); return; }

        PLOGE << "no found";

    });

    NODE<UberLayer>::is_a<Layer>();
    NODE<Layer>::is_a<Modelable>();
    NODE<Model>::is_a<Modelable>();
    NODE<Modelable>::is_a<Effectable>();
    NODE<UberLayer::VLayer>::is_a<Effectable>();

    NODE<Layer>::oncreate([](Node* node, Layer *layer){ 
        NODE<Struct>::oncreate_cb(node, &layer->s);  
    }); 

    NODE<Layer>::onchange([](Node* node, Layer *layer){ NODE<Struct>::onchange_cb(node, &layer->s); layer->update(); });

    NODE<Layer>::onrun([](Node* node, Layer *layer){ layer->draw(); });

    NODE<Modelable>::onadd<File>([](Node*_this,Node*node){

        return _this->addPtr<Model>(_this->is_a<Modelable>()->addModel( node->is_a<File>() ));

    });

    NODE<Effectable>::onadd<Effector>([](Node*_this,Node*node){ 
        
        return _this->addPtr<EffectorRef>( _this->is_a<Effectable>()->addEffector( node->is_a<Effector>() ) ); 
        
    });

    ////////// Effector.HPP

    NODE<FileEffector>::is_a<Effector>();
    NODE<Wrappy>::is_a<Effector>();

    NODE<Effector>::oncreate([](Node*node, Effector* def){ NODE<Struct>::oncreate_cb(node, &def->s); });

    NODE<EffectorRef>::oncreate([](Node*node, EffectorRef* fx){ NODE<Struct>::oncreate_cb(node, &fx->s); });

    NODE<EffectorRef>::onchange([&](Node*node, EffectorRef* effector){ NODE<Struct>::onchange_cb(node, &effector->s); effector->update(); });

    NODE<Effector>::onadd<Effector>([](Node* _this, Node *node) {


        Effector * from_effector = _this->is_a<Effector>();
        auto wrap = dynamic_cast<Wrappy*>(from_effector);
        Effector * to_effector = node->is_a<Effector>();

        if (!wrap) return _this;

        wrap->addEffector(to_effector);

        return _this;

    });

    NODE<EffectorRef>::ondelete([](Node* node, EffectorRef *effector) {

        auto lay = node->parent()->is_a_nowarning<Layer>();
        if (lay) lay->removeEffector(effector); 

        auto ubl = node->parent()->is_a_nowarning<UberLayer::VLayer>();
        if (ubl) ubl->removeEffector(effector);

        auto model = node->parent()->is_a_nowarning<Model>();
        if (model) model->removeEffector(effector);

    });

    ////////// Atlas.HPP

    NODE<Atlas>::oncreate([](Node* node, Atlas *atlas) { 
        node->name(atlas->path);
        });
    NODE<Atlas>::onchange([](Node* node, Atlas *atlas) { 
        atlas->fromDir(atlas->path); 
        });

    ////////// Artnet.HPP

    NODE<Artnet>::onrun([](Node* node, Artnet *an){ an->run();

        static int size = 0;

        // if (an->universes.size() != size) {
        //     size = an->universes.size();
        //     // node->get<Artnet>()->trigchange();
        // }
    });
    

    NODE<Artnet>::onchange([](Node* node, Artnet *an){

        NODE<Struct>::onchange_cb(node, an);


        // for (auto c :node->childrens) delete c;

        // // for (auto &uni :an->universes) {

        // //     uni.second->id = uni.first;

        // //     node->addPtr<Universe>(uni.second)->name("universe "+std::to_string(uni.first));

        // // }

    });
    //////// Remap.HPP

    // NODE<DMXRemap>::onrun([](Node* node, DMXRemap *remap) {

    //     int count = 0;
    //     remap->dst->def()->each([&](Instance &inst){count++;});
    //     if (count != remap->attributes.size()) remap->attributes.resize(count);

    // });


    // NODE<Remap>::onrun([](Node* node, Remap *remap) { remap->update(); });
    // NODE<Remap>::onchange([](Node* node, Remap *remap) { remap->reset(); });
    // NODE<Universe::Remap>::onrun([](Node* node, Universe::Remap *remap) { remap->update(); });
    // NODE<Universe::Remap>::onchange([](Node* node, Universe::Remap *remap) { remap->reset(); });

    //////// FrameBuffer.HPP

    NODE<FrameBuffer>::onchange([](Node* node, FrameBuffer *fb) { if (fb->width != fb->texture->width || fb->height != fb->texture->height) { fb->create(fb->width, fb->height); } });

    //////// Buffer.HPP

    NODE<Buffer>::onchange([](Node* node, Buffer *buffer) { PLOGD<<"ooo"; });

    ////////// Folder.HPP

    NODE<Folder>::oncreate([](Node* node, Folder *dir){ node->name(dir->path); });

    ////////// Output

    NODE<Output>::onrun([](Node* node, Output *output){ output->draw(); });

    NODE<Window>::is_a<Output>();

    NODE<NDI::Sender>::is_a<Output>();

    ////////// NDI

    NODE<NDI::Sender>::oncreate([](Node* node, NDI::Sender *sender){ node->name(sender->name);  });

    NODE<NDI::Sender>::onchange([](Node* node, NDI::Sender *sender){

        if (!sender->fb) return;

        if (sender->fb->width != sender->width || sender->fb->height != sender->height) { sender->size(sender->fb->width, sender->fb->height); }

      });

    ////////// JSON.HPP

    NODE<JSON>::oncreate([](Node* node, JSON *json){ if (json->file) node->name(json->file->name()); });

}
