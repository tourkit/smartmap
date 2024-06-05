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

    //     auto s = _this->is_a<xxx>();
    //     auto f = node->is_a<yyy>();

    //     return _this->UntypedNode::add(node);

    // });

    // NODE<Folder>::oncreate([](Node* node, Folder *folder){




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

    NODE<Struct>::oncreate([](Node* node, Struct *s){ node->name(s->name()); });

    NODE<Struct>::onchange([&](Node*node, Struct* s){ s->name(node->name()); });

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


    ////////// UBO.HPP

    NODE<UBO>::onrun([](Node* node, UBO *ubo){ ubo->upload(); });

    NODE<UBO>::oncreate([](Node* node, UBO *ubo){ node->name(ubo->name()); });


    ////////// ENGINE.HPP (and Stack)

    NODE<Stack>::onadd<File>([](Node*_this,Node*node){

        auto x = _this->addOwnr<Layer>();
        x->add(node)->active(false);
        return x->node();

    });

    NODE<Stack>::onchange([](Node*node,Stack*stack){

        engine.tree->each([](Node* n) {

            ShaderProgram* shader = nullptr;
            if (n->is_a_nowarning<Layer>()) shader = &n->is_a<Layer>()->shader;
            else if (n->is_a_nowarning<UberLayer>()) shader = &n->is_a<UberLayer>()->shader;
            else if (n->is_a_nowarning<ShaderProgram>()) shader = n->is_a<ShaderProgram>();
            else return;

            if (shader) shader->create();

        });

     });

    ////////// LAYER.HPP

    ////////// UberLayer.HPP

    NODE<UberLayer>::oncreate([](Node* node, UberLayer *ubl){ NODE<Struct>::oncreate_cb(node, &ubl->s);  for (auto &x : ubl->layers) { node->addPtr<UberLayer::VLayer>(&x); } });

    NODE<UberLayer>::onchange([](Node* node, UberLayer *ubl){ NODE<Struct>::onchange_cb(node, &ubl->s); ubl->update(); });

    NODE<UberLayer>::onrun([](Node* node, UberLayer *ubl){ ubl->draw(); });


    NODE<UberLayer::VLayer>::onchange([](Node* node, UberLayer::VLayer *layer){ NODE<Struct>::onchange_cb(node, &layer->s); });
    NODE<UberLayer::VLayer>::oncreate([](Node* node, UberLayer::VLayer *layer){ NODE<Struct>::oncreate_cb(node, &layer->s); });
    // NODE<UberLayer::VLayer>::onadd<File>([](Node*_this,Node*node){

    //     auto file = node->is_a<File>();

    //     auto x =  _this->is_a<UberLayer::VLayer>()->addEffector( file )  ;

    //     if (!x) return _this;

    //     return _this->addPtr<Effector>( x )->node();

    // });

    ////////// DRAWCALL.HPP

    NODE<Layer>::oncreate([](Node* node, Layer *layer){ NODE<Struct>::oncreate_cb(node, &layer->s);  }); // for what ??????

    NODE<Layer>::onchange([](Node* node, Layer *layer){ NODE<Struct>::onchange_cb(node, &layer->s); layer->update(); });

    NODE<Layer>::onrun([](Node* node, Layer *layer){ layer->draw(); });

    NODE<Layer>::onadd<File>([](Node*_this,Node*node){

        auto file = node->is_a<File>();

        if (file->extension == "glsl") return _this->addPtr<Effector>( _this->is_a<Layer>()->addEffector(file) )->node();

        return _this->addPtr<Model>(_this->is_a<Layer>()->addModel( file ))->node();

        });


    ////////// MODEL.HPP

    NODE<Model>::oncreate([](Node*node, Model* mod){ NODE<Struct>::oncreate_cb(node, &mod->s); });

    NODE<Model>::onchange([&](Node*node, Model* mod){ NODE<Struct>::onchange_cb(node, &mod->s); /*PLOGD << engine.dynamic_ubo.print_recurse();*/ });

    NODE<Model>::onadd<File>([](Node*_this,Node*node){

        auto file = node->is_a<File>();

        auto x =  _this->is_a<Model>()->addEffector( file )  ;

        if (!x) return _this;

        return _this->addPtr<Effector>( x )->node();

    });

    NODE<Model>::ondelete([](Node* node, Model *model) {

        auto dc = node->parent()->is_a<DrawCall>();
        if (dc) dc->removeModel(model);

        auto layer = node->parent()->is_a<Layer>();
        if (layer) layer->removeModel(model);

    });

    ////////// Effector.HPP

    NODE<Effector>::oncreate([](Node*node, Effector* fx){ NODE<Struct>::oncreate_cb(node, &fx->ref); });

    NODE<Effector>::onchange([&](Node*node, Effector* fx){ NODE<Struct>::onchange_cb(node, &fx->ref); });

    NODE<Effector>::ondelete([](Node* node, Effector *effector) {

        auto model = node->parent()->is_a_nowarning<Model>();

        if (!model) { node->parent()->is_a<Layer>()->removeEffector(effector); return ;}

        model->removeEffector(effector);

    });

    ////////// Atlas.HPP

    NODE<Atlas>::onchange([](Node* node, Atlas *atlas) { atlas->fromDir(atlas->path); });

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

    ////////// Output.HPP

    NODE<Output>::onrun([](Node* node, Output *output){ output->draw(); });

    ////////// Window.HPP

    NODE<Window>::onrun([](Node* node, Window *window){ NODE<Output>::onrun_cb(node, window); });

    ////////// NDI.HPP

    NODE<NDI::Sender>::onrun([](Node* node, NDI::Sender *sender){ NODE<Output>::onrun_cb(node, sender); });

    NODE<NDI::Sender>::oncreate([](Node* node, NDI::Sender *sender){ node->name(sender->name);  });

    NODE<NDI::Sender>::onchange([](Node* node, NDI::Sender *sender){

        if (!sender->layer) return;

        if (sender->layer->fb.width != sender->width || sender->layer->fb.height != sender->height) { sender->size(sender->layer->fb.width, sender->layer->fb.height); }

      });

    ////////// JSON.HPP

    NODE<JSON>::oncreate([](Node* node, JSON *json){ if (json->file) node->name(json->file->name()); });

}
