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

    // NODE<xxx>::on(Node::CREATE, [](Node* node, xxx *x){ });
    // NODE<xxx>::on(Node::CHANGE, [](Node* node, xxx *x){ });
    // NODE<xxx>::on(Node::RUN, [](Node* node, xxx *x){ });

    // NODE<xxx>::onadd<yyy>([](Node*_this,Node*node){

    //     auto x = _this->is_a<xxx>();
    //     auto y = node->is_a<yyy>();

    //     return _this->Node::add(node);

    // });
    
    ////////// FILE.HPP

    NODE<File>::on(Node::CREATE, [](Node* node, File *file){ node->name(file->name()); });

    NODE<File>::on(Node::RUN, [](Node* node, File *file){

        // if (file->last_modified != file->getTimeModified()) file->reload();
        if (file->hasChanged()) {
            file->reload();
            node->update();


            }

    });

    ////////// Struct.HPP

    NODE<Struct>::on(Node::CREATE, [](Node* node, Struct *s){ 
        node->name_v = (s->name()); 
    });

    NODE<Struct>::on(Node::CHANGE, [&](Node*node, Struct* s){
        if (s->name() != node->name())
            s->name(node->name()); });



    ////////// ENGINE

    NODE<Buffer>::on(Node::RUN, [](Node* node, Buffer *buff){ buff->upload(); });
    
    NODE<UBO>::is_a<Buffer>();

    NODE<Buffer>::is_a<Struct>();


    ////////// DRAWCALL

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){

        auto x = _this->addOwnr<Layer>();
        x->add(node)->active(false);
        return x;

    });
    
    NODE<Stack>::on(Node::CHANGE, [](Node*node, Stack* stack){

        for (auto c : node->childrens) {

            Layer* layer = c->is_a<Layer>();

            // if (layer) layer->shader.create();
        }
        
    });

    NODE<Layer>::on(Node::DELETE, [](Node* node, Layer *layer){ 

        for (auto x : node->referings) {

            auto o = x->is_a<Output>();

            if (o && o->fb == &layer->fb) o->fb = nullptr;
 
        } 
        
    }); 

    NODE<Model>::on(Node::DELETE, [](Node* node, Model *model) {

        auto dc = node->parent()->is_a_nowarning<DrawCall>();
        if (dc) { dc->removeModel(model); return; }

        auto layer = node->parent()->is_a_nowarning<Layer>();
        if (layer) { layer->removeModel(model); return; }

        PLOGE << "no found";

    });

    NODE<UberLayer>::is_a<Layer>();
    NODE<Layer>::is_a<Modelable>();
    NODE<Layer>::is_a<DrawCall>();
    NODE<Model>::is_a<Modelable>();
    NODE<Modelable>::is_a<Effectable>();
    NODE<UberLayer::VLayer>::is_a<Effectable>();

    NODE<Modelable>::on(Node::CREATE, [](Node* node, Modelable *m){ 
    
        NODE<Struct>::on_cb[Node::CREATE](node, &m->s);
        
    });

    // NODE<DrawCall>::on(Node::CHANGE, [](Node* node, DrawCall *dc){ dc->shader.create(); });
    NODE<Layer>::on(Node::CHANGE, [](Node* node, Layer *layer){ 
        layer->update();
    });

    NODE<Layer>::on(Node::RUN, [](Node* node, Layer *layer){ 
        layer->draw(); 
    });

    NODE<Modelable>::onadd<File>([](Node*_this,Node*node){
        auto f = node->is_a<File>() ;
        return _this->addPtr<Model>(_this->is_a<Modelable>()->addModel( f))->name(f->name());

    });

    NODE<Effectable>::onadd<Effector>([](Node*_this,Node*node){ 
        
        return _this->addPtr<EffectorRef>( _this->is_a<Effectable>()->addEffector( node->is_a<Effector>() ) ); 
        
    });

    ////////// Effector.HPP

    NODE<FileEffector>::is_a<Effector>();
    NODE<Wrappy>::is_a<Effector>();

    NODE<Wrappy>::onadd<Effector>([](Node* _this, Node *node) {

        _this->is_a<Wrappy>()->addEffector(node->is_a<Effector>());

        return _this;

    });

    NODE<Effector>::on(Node::CREATE, [](Node*node, Effector* def){ NODE<Struct>::on_cb[Node::CREATE](node, &def->s); });

    NODE<EffectorRef>::on(Node::CREATE, [](Node*node, EffectorRef* fx){ NODE<Struct>::on_cb[Node::CREATE](node, &fx->s); });

    NODE<EffectorRef>::on(Node::CHANGE, [&](Node*node, EffectorRef* effector){ NODE<Struct>::on_cb[Node::CHANGE](node, &effector->s);  effector->update(); });

    NODE<EffectorRef>::on(Node::DELETE, [](Node* node, EffectorRef *effector) {

        auto lay = node->parent()->is_a_nowarning<Layer>();
        if (lay) lay->removeEffector(effector); 

        auto ubl = node->parent()->is_a_nowarning<UberLayer::VLayer>();
        if (ubl) ubl->removeEffector(effector);

        auto model = node->parent()->is_a_nowarning<Model>();
        if (model) model->removeEffector(effector);

    });

    ////////// Atlas.HPP

    NODE<Atlas>::on(Node::CREATE, [](Node* node, Atlas *atlas) { 
        node->name(atlas->path);
        });
    NODE<Atlas>::on(Node::CHANGE, [](Node* node, Atlas *atlas) { 
        atlas->fromDir(atlas->path); 
        });

    ////////// Artnet.HPP

    NODE<Artnet>::on(Node::RUN, [](Node* node, Artnet *an){ an->run();

        static int size = 0;

        // if (an->universes.size() != size) {
        //     size = an->universes.size();
        //     // node->get<Artnet>()->trig(Node::CHANGE);
        // }
    });
    

    NODE<Artnet>::on(Node::CHANGE, [](Node* node, Artnet *an){

        NODE<Struct>::on_cb[Node::CHANGE](node, an);


        // for (auto c :node->childrens) delete c;

        // // for (auto &uni :an->universes) {

        // //     uni.second->id = uni.first;

        // //     node->addPtr<Universe>(uni.second)->name("universe "+std::to_string(uni.first));

        // // }

    });
    //////// Remap.HPP

    // NODE<DMXRemap>::on(Node::RUN, [](Node* node, DMXRemap *remap) {

    //     int count = 0;
    //     remap->dst->def()->each([&](Instance &inst){count++;});
    //     if (count != remap->attributes.size()) remap->attributes.resize(count);

    // });


    // NODE<Remap>::on(Node::RUN, [](Node* node, Remap *remap) { remap->update(); });
    // NODE<Remap>::on(Node::CHANGE, [](Node* node, Remap *remap) { remap->reset(); });
    // NODE<Universe::Remap>::on(Node::RUN, [](Node* node, Universe::Remap *remap) { remap->update(); });
    // NODE<Universe::Remap>::on(Node::CHANGE, [](Node* node, Universe::Remap *remap) { remap->reset(); });

    //////// FrameBuffer.HPP

    NODE<FrameBuffer>::on(Node::CHANGE, [](Node* node, FrameBuffer *fb) { if (fb->width != fb->texture->width || fb->height != fb->texture->height) { fb->create(fb->width, fb->height); } });

    //////// Buffer.HPP

    NODE<Buffer>::on(Node::CHANGE, [](Node* node, Buffer *buffer) { PLOGD<<"ooo"; });

    ////////// Folder.HPP

    NODE<Folder>::on(Node::CREATE, [](Node* node, Folder *dir){ node->name(dir->path); });

    ////////// Output

    NODE<Output>::on(Node::RUN, [](Node* node, Output *output){ output->draw(); });

    NODE<Window>::is_a<Output>();

    NODE<NDI::Sender>::is_a<Output>();

    ////////// NDI

    NODE<NDI::Sender>::on(Node::CREATE, [](Node* node, NDI::Sender *sender){ node->name(sender->name);  });

    NODE<NDI::Sender>::on(Node::CHANGE, [](Node* node, NDI::Sender *sender){

        if (!sender->fb) return;

        if (sender->fb->width != sender->width || sender->fb->height != sender->height) { sender->size(sender->fb->width, sender->fb->height); }

      });

    ////////// JSON.HPP

    NODE<JSON>::on(Node::CREATE, [](Node* node, JSON *json){ if (json->file) node->name(json->file->name()); });

}
