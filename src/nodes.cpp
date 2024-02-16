#include "nodes.hpp"

#include "node.hpp"
#include "file.hpp"
#include "ubo.hpp"
#include "struct.hpp"
#include "engine.hpp"
#include "model.hpp"
#include "shaderfx.hpp"
#include "directory.hpp"
#include "drawcall.hpp"

//check none have gui in.hpp !!

void Nodes::init() {

    // ////////// xxx.HPP 
    
    // On<xxx>::create([](Node* node, xxx *x){ });

    // On<xxx>::add<yyy>([](Node*_this,Node*node){ 

    //     auto s = ((Ptr<xxx>*)_this)->get();
    //     auto f = ((Ptr<yyy>*)node)->get();

    //     return node;
        
    // });

    ////////// FILE.HPP 

    On<File>::create([](Node* node, File *file){ node->name = file->name+"."+file->extension+""; });

    On<File>::run([](Node* node, File *file){ 

        if (file->hasChanged()) { 

            file->reload(); 

            engine.tree.runCB([file](Node* node){

                if (node->ptr_untyped() == file) PLOGD << node->name <<" is a ref of " << file->name << " . in " << node->parent()->name;

            });
            
        }

     });

    ////////// UBO.HPP 

    On<UBO>::run([](Node* node, UBO *ubo){ ubo->upload(); });
    
    On<UBO>::create([](Node* node, UBO *ubo){ node->name = ubo->name; });

    ////////// STRUCT.HPP 

    On<Struct>::create([](Node* node, Struct *s){ node->name = s->name; });

    ////////// ENGINE.HPP (and Stack)

    On<Stack>::add<DrawCall>([](Node*_this,Node*node){ 

        node->is_a<DrawCall>()->update();

        return _this->UntypedNode::add(node); // missind add() use case (still ? maybe not untyped)

        // maybe moved stored_type to UntypedNode and use if!=UntypedNode get name and so on

    });

    On<Stack>::add<UBO>([](Node*_this,Node*node){ 

        return _this->UntypedNode::add(node);


    });


    On<Stack>::add<File>([](Node*_this,Node*node){ 

        return _this->UntypedNode::add(node);


    });

    ////////// DRAWCALL.HPP 

    On<DrawCall>::run([](Node* node, DrawCall *dc){  dc->run(); });
    
    // On<DrawCall>::add<Model>([](Node*_this,Node*node){ return _this->UntypedNode::add(node); });

    On<DrawCall>::add<File>([](Node*_this,Node*node){ 
        
        auto dc = _this->is_a<DrawCall>();
        auto file = node->is_a<File>();
        if (!dc || !file) return node;

        auto model = dc->vbo.import(file);
        
        dc->update();

        // return _this->addPtr<Model>(model);
        
        return _this->UntypedNode::add(new Ptr<Model>(model));

    });

    ////////// MODEL.HPP 

    On<Model>::create([](Node* node, Model *model) { node->name = model->file->name; });

    On<Model>::add<ShaderFX>([](Node*_this,Node*node){ return _this->UntypedNode::add(node); });

    On<Model>::add<File>([](Node*_this,Node*node){ 
        
        auto model = _this->is_a<Model>();
        auto file = node->is_a<File>();
        if (!model || !file) return node;

        auto bad = new ShaderFX(file); // unowned...

        model->addFX(bad);

        auto dc = _this->parent()->is_a<DrawCall>();
        if (dc) dc->update();
        
        return _this->Node::add(new Ptr<ShaderFX>(bad));

    });
    
    On<ShaderFX>::create([](Node* node, ShaderFX *fx) { node->name = fx->file->name; });


    ////////// Directory.HPP 

    On<Directory>::create([](Node* node, Directory *dir){ 

        node->name = dir->path;

        for (auto f : dir->list) node->addOwnr<File>(f);
        
    });

}