#include "callbacks.hpp"

#include "engine.hpp"
#include "member.hpp"
#include "node.hpp"
#include "file.hpp"
#include "remap.hpp"
#include "shader.hpp"
#include "ubo.hpp"
#include "model.hpp"
#include "effector.hpp"
#include "folder.hpp"
#include "drawcall.hpp"
#include "artnet.hpp"
#include "ndi.hpp"
#include "atlas.hpp"
#include "json.hpp"
#include "window.hpp"
#include "framebuffer.hpp"
#include "layer.hpp"
#include "texture.hpp"


#include "engine.hpp"

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
            m->ref()->name(node->name()); 
    });
    
    NODE<Member>::on(Node::RUN, [](Node* node, Member *m){ m->upload(); });

    NODE<UBO>::is_a<Member>();

    ////////// DRAWCALL

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){

        auto x = _this->addOwnr<Layer>();
        x->add(node);
        return x;

    });
    
    NODE<Layer>::on(Node::CHANGE, [](Node* node, Layer *layer){ 

        layer->glsl_layers->eq(layer->vbo.layer_id).set<std::array<float,2>>({(float)layer->fb.width,(float)layer->fb.height});

    });

    NODE<Layer>::on(Node::DESTROY, [](Node* node, Layer *layer){ 

        for (auto x : node->referings) {

            auto o = x->is_a<Output>();

            if (o && o->fb == &layer->fb) o->fb = nullptr;
 
        } 
        
    }); 

    NODE<Model>::on(Node::DESTROY, [](Node* node, Model *model) {

        auto modelable = node->parent()->is_a_nowarning<Modelable>();

        if (modelable) { 

            modelable->removeModel(model); 

            return; 

        }

        PLOGE << "no found";
 
    });

    NODE<UberLayer>::allow<UberLayer::VirtualLayer>();
    NODE<Effectable>::allow<EffectorRef>();
    NODE<Modelable>::allow<Model>();
    NODE<Artnet>::allow<Universe>();
    NODE<Universe>::allow<DMXRemap>();
    
    NODE<UberLayer>::is_a<Layer>();
    NODE<Layer>::is_a<DrawCall>();
    NODE<Model>::is_a<Effectable>();
    NODE<UberLayer::VirtualLayer>::is_a<Effectable>();
    NODE<Effectable>::is_a<Member>();
    NODE<Effector>::is_a<Member>();

    NODE<Modelable>::is_a<Effectable>();

    NODE<DrawCall>::is_a<Modelable>();

    NODE<DrawCall>::on(Node::RUN, [](Node* node, DrawCall *dc) { 

        dc->draw(); 
        
    });

    NODE<Layer::Feedback>::on(Node::CHANGE, [](Node* node, Layer::Feedback *feedback) {


        feedback->texture.create( feedback->layer->fb.width, feedback->layer->fb.height );


    });

    NODE<DrawCall>::on(Node::CHANGE, [](Node* node, DrawCall *dc) {

        node->each<DrawCall>([](Node*n, DrawCall* dc){ 
            
            dc->builder()->build(&dc->shader);

        });

        dc->vbo.upload();

    });

    NODE<Effectable>::onadd<File>([](Node*_this,Node*node){

        auto file = node->is_a<File>();
        if (file->extension != "glsl") {

            if (_this->is_a_nowarning<Modelable>())
                return Node::no_worry;

            else
                return Node::Null;

        }

        NODE<File>::allow<Effector>();

        Node* effector = nullptr;

        // bool found = false;

        // // check if des hidden
        // // make break work

        // node->eachB<FileEffector>(([&](Node* n, FileEffector* effector_){
            
        //     effector = n;
            
        //     return Node::Break;

        // }));

        // if (!effector)
            effector = node->addOwnr<FileEffector>(file);

        return _this->add(effector);
    });
    NODE<Modelable>::onadd<File>([](Node*_this,Node*node){

        auto file = node->is_a<File>();
        if (file->extension != "obj") 
            return Node::Null;

        return _this->addPtr<Model>(_this->is_a<Modelable>()->addModel( node->is_a<File>() ));

    });

    NODE<Effectable>::onadd<Effector>([](Node*_this,Node*node){ 

        auto effectable =  _this->is_a<Effectable>();
        auto effector =  node->is_a<Effector>();
        auto x = effectable->addEffector( effector );
        auto n = _this->addPtr<EffectorRef>(x);
        
        return n;
        
    });

    NODE<Effectable>::onadd<UberLayer>([](Node*_this,Node*node){ 
        
        return _this->addPtr<EffectorRef>( _this->is_a<Effectable>()->addEffector(&node->is_a<UberLayer>()->effector));
        
    });
    NODE<Effectable>::onadd<UberLayer::VirtualLayer>([](Node*_this,Node*node){ 
        
        auto out = _this->addPtr<EffectorRef>( _this->is_a<Effectable>()->addEffector(&node->is_a<UberLayer::VirtualLayer>()->effector));

        out->name(node->name());
        
        return out;
        
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

    NODE<UberLayer>::on(Node::CHANGE, [&](Node*node, UberLayer* ubl){ NODE<Member>::on_cb[Node::CHANGE](node, &ubl->effector);   });

    // NODE<EffectorRef>::on(Node::CREATE, [](Node*node, EffectorRef* fx){ NODE<Member>::on_cb[Node::CREATE](node, fx->ref());  });
    
    // NODE<EffectorRef>::on(Node::CHANGE, [&](Node*node, EffectorRef* ref){   });
    NODE<EffectorRef>::is_a<Member>();

    NODE<EffectorRef>::on(Node::DESTROY, [](Node* node, EffectorRef *effector) {

            engine.window.end_of_render_cbs.emplace_back(node,[](void* ptr){

                auto node = (Node*)ptr;

                auto effectable = node->parent()->is_a_nowarning<Effectable>();
                if (effectable) 
                effectable->removeEffector(node->is_a_nowarning<EffectorRef>()); 

            });

        

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


    NODE<Output>::on(Node::CHANGE, [](Node* node, Output *output){

        if (!output->fb) return;

        if (output->fb->width != output->width || output->fb->height != output->height) { 

            PLOGE << "resize " << node->name() << " from " << output->width << "x" << output->height << " to " << output->fb->width << "x" << output->fb->height;

            output->size(output->fb->width, output->fb->height); 
        }

      });
    
    NODE<Output>::onadd<Layer>([](Node* _this, Node *node) {

        auto output = _this->is_a<Output>();

        if (output->fb) {

            Node* found = nullptr;

            for (auto r : _this->referings) {

                auto layer = r->is_a<Layer>();
                
                if (layer && &layer->fb == output->fb) {

                    found = node;

                    break;

                }

            }

            if (found)
                _this->referings.erase(found);
            else
                PLOGW << "unknown fb owner";
        
        }
       
        output->fb = &node->is_a<Layer>()->fb;
        
        _this->referings.insert(node);

        return Node::no_worry;

    });

    NODE<Window>::is_a<Output>();

    NODE<NDI::Sender>::is_a<Output>();

    ////////// NDI

    NODE<NDI::Sender>::on(Node::CREATE, [](Node* node, NDI::Sender *sender){ node->name(sender->name);  });

    ////////// JSON.HPP

    NODE<JSON>::on(Node::CREATE, [](Node* node, JSON *json){ if (json->file) node->name(json->file->name()); });

}
