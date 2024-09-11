#include "callbacks.hpp"

#include "node.hpp"
#include "file.hpp"
#include "remap.hpp"
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

    ////////// Member.HPP

    NODE<Member>::on(Node::CREATE, [](Node* node, Member *m){ 
        node->name_v = (m->ref()->name()); 
    });

    NODE<Member>::on(Node::CHANGE, [&](Node*node, Member* m){
        if (m->name() != node->name())
            m->ref()->name(node->name()); });



    ////////// ENGINE
    
    NODE<Member>::on(Node::RUN, [](Node* node, Member *m){ m->upload(); });

    NODE<UBO>::is_a<Member>();

    ////////// DRAWCALL

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){

        auto x = _this->addOwnr<Layer>();
        x->add(node)->active(false);
        return x;

    });
    
    NODE<Stack>::on(Node::CHANGE, [](Node*node, Stack* stack){

        node->each<Layer>([](Node*n, Layer* layer){ 

            layer->shader.create(); 

        });
        
    });

    NODE<Layer>::on(Node::DESTROY, [](Node* node, Layer *layer){ 

        for (auto x : node->referings) {

            auto o = x->is_a<Output>();

            if (o && o->fb == &layer->fb) o->fb = nullptr;
 
        } 
        
    }); 

    NODE<Model>::on(Node::DESTROY, [](Node* node, Model *model) {

        auto dc = node->parent()->is_a_nowarning<DrawCall>();
        if (dc) { dc->removeModel(model); return; }

        auto layer = node->parent()->is_a_nowarning<Layer>();
        if (layer) { layer->removeModel(model); return; }

        PLOGE << "no found";
 
    });

    NODE<DrawCall>::on(Node::CHANGE, [](Node* node, DrawCall *dc) {

        dc->update();

    });

    NODE<UberLayer>::allow<UberLayer::VirtualLayer>();
    NODE<Effectable>::allow<EffectorRef>();
    NODE<Modelable>::allow<Model>();
    NODE<Artnet>::allow<Universe>();
    NODE<Universe>::allow<DMXRemap>();
    NODE<UberLayer>::is_a<Layer>();
    NODE<DrawCall>::is_a<Modelable>();
    NODE<Layer>::is_a<DrawCall>();
    NODE<Model>::is_a<Modelable>();
    NODE<UberLayer::VirtualLayer>::is_a<Effectable>();
    NODE<Modelable>::is_a<Effectable>();

    NODE<Layer>::on(Node::RUN, [](Node* node, Layer *layer){ 

        layer->draw(); 
        
    });

    NODE<DrawCall>::on(Node::RUN, [](Node* node, DrawCall *dc) { 

        dc->draw(); 
        
    });

    NODE<Effectable>::on(Node::CREATE, [](Node* node, Effectable *e){ 
    
        NODE<Member>::on_cb[Node::CREATE](node, &e->Effectable::m);
        
    });

    NODE<Effectable>::on(Node::CHANGE, [](Node* node, Effectable *e){ 
    
        NODE<Member>::on_cb[Node::CHANGE](node, &e->Effectable::m);
        
    });


    NODE<Modelable>::onadd<File>([](Node*_this,Node*node){

        return _this->addPtr<Model>(_this->is_a<Modelable>()->addModel( node->is_a<File>() ));

    });

    NODE<Effectable>::onadd<Effector>([](Node*_this,Node*node){ 

        auto x = _this->addPtr<EffectorRef>( _this->is_a<Effectable>()->addEffector( node->is_a<Effector>() ));

        // node->referings.insert(x);
        
        return x;
        
    });

    NODE<Effectable>::onadd<UberLayer>([](Node*_this,Node*node){ 
        
        return _this->addPtr<EffectorRef>( _this->is_a<Effectable>()->addEffector(&node->is_a<UberLayer>()->effector));
        
    });

    ////////// Effector.HPP

    // NODE<UberEffector>::is_a<Effector>();
    NODE<FileEffector>::is_a<Effector>();
    NODE<Wrappy>::is_a<Effector>();
    NODE<Wrappy>::is_a<Effectable>();

    NODE<Wrappy>::onadd<Effector>([](Node* _this, Node *node) {

        _this->is_a<Wrappy>()->addEffector(node->is_a<Effector>());
        
        return _this;

    });

    NODE<Effector>::on(Node::CREATE, [](Node*node, Effector* def){ NODE<Member>::on_cb[Node::CREATE](node, &def->m); });


    // NODE<Effector>::on(Node::CHANGE, [&](Node*node, Effector* effector){ NODE<Member>::on_cb[Node::CHANGE](node, &effector->m);  });

    NODE<UberLayer>::on(Node::CHANGE, [&](Node*node, UberLayer* ubl){ NODE<Member>::on_cb[Node::CHANGE](node, &ubl->effector.m);   });

    NODE<EffectorRef>::on(Node::CREATE, [](Node*node, EffectorRef* fx){ NODE<Member>::on_cb[Node::CREATE](node, fx->m.ref());  });
    
    NODE<EffectorRef>::on(Node::CHANGE, [&](Node*node, EffectorRef* ref){  ref->update(); });

    NODE<EffectorRef>::on(Node::DESTROY, [](Node* node, EffectorRef *effector) {

        auto lay = node->parent()->is_a_nowarning<Layer>();
        if (lay) lay->removeEffector(effector); 

        auto ubl = node->parent()->is_a_nowarning<UberLayer::VirtualLayer>();
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

        if (an->universes.size() != size) {

            std::vector<Universe*> missing;

            for (auto uni : an->universes) {

                bool found = false;
                for (auto n : node->childrens) {
                    auto uni_ = n->is_a_nowarning<Universe>();
                    if (uni_ == uni.second.get()) {
                        found = true;
                        break;
                    }

                }

                if (!found) 
                    missing.push_back(uni.second.get());

            }

            for (auto x : missing)
                node->addPtr<Universe>(x)->name("universe "+std::to_string(x->id));
            
        
            missing.clear();

            size = an->universes.size();
        }

    });
    
    // NODE<Artnet>::is_a<Member>();

    NODE<Artnet>::on(Node::CHANGE, [](Node* node, Artnet *an){


    });

    //////// Remap.HPP

    NODE<Remap>::on(Node::RUN, [](Node* node, Remap *remap) { remap->update(); });

    NODE<DMXRemap>::is_a<Remap>();

    //////// FrameBuffer.HPP

    NODE<FrameBuffer>::on(Node::CHANGE, [](Node* node, FrameBuffer *fb) { if (fb->width != fb->texture.width || fb->height != fb->texture.height) { fb->create(fb->width, fb->height); } });


    ////////// Folder.HPP

    NODE<Folder>::on(Node::CREATE, [](Node* node, Folder *dir){ node->name(dir->path); });

    ////////// Output

    NODE<Output>::on(Node::RUN, [](Node* node, Output *output){ output->draw(); });
    
    NODE<Output>::onadd<Layer>([](Node* _this, Node *node) {

        auto output = _this->is_a<Output>();

        if (output->fb) {

            Node* found = nullptr;

            engine.stack->each<Layer>([&](Node* node, Layer* layer) {

                if (&layer->fb == output->fb)
                    found = node;

            });

            if (found)
                _this->referings.erase(found);
            else
                PLOGW << "unknown fb owner";
        
        }
       
        output->fb = &node->is_a<Layer>()->fb;
        
        _this->referings.insert(node);

        return nullptr;

    });

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
