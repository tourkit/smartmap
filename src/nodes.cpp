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
    
    // UntypedNode::oncreate<xxx>([](AnyNode* node, xxx *x){ });

    // TypedNode<xxx>::onadd<yyy>([](AnyNode*_this,AnyNode*node){ 

    //     auto s = ((Ptr<xxx>*)_this)->get();
    //     auto f = ((Ptr<yyy>*)node)->get();

    //     return node;
        
    // });

    ////////// FILE.HPP 

    UntypedNode::oncreate<File>([](AnyNode* node, File *file){ node->name = file->name+"."+file->extension+""; });

    static std::map<int,int> filechecks;

    UntypedNode::onrun<File>([](AnyNode* node, File *file){ 

        int uid = 0;

        WIN32_FILE_ATTRIBUTE_DATA fileInfo; 
        
        GetFileAttributesExA(file->path.c_str(), GetFileExInfoStandard, &fileInfo);

        SYSTEMTIME st; 
        
        FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &st);

        auto last = st.wMilliseconds;

        if (filechecks[uid] != last) { filechecks[uid] = last;  file->reload(); }

     });

    ////////// UBO.HPP 

    UntypedNode::onrun<UBO>([](AnyNode* node, UBO *ubo){ ubo->upload(); });
    
    UntypedNode::oncreate<UBO>([](AnyNode* node, UBO *ubo){ node->name = ubo->name; });

    ////////// STRUCT.HPP 

    UntypedNode::oncreate<Struct>([](AnyNode* node, Struct *s){ node->name = s->name; });

    ////////// ENGINE.HPP (and Stack)

    TypedNode<Stack>::onadd<DrawCall>([](AnyNode*_this,AnyNode*node){ 

        // auto dc = ((Ptr<DrawCall>*)node)->get();
        // dc->update();
    
        // _this->UntypedNode::add(node);
        // return _this; 
        
        return nullptr;

    });

    TypedNode<Stack>::onadd<UBO>([](AnyNode*_this,AnyNode*node){ 

        _this->UntypedNode::add(node);
        return node;


    });

    ////////// DRAWCALL.HPP 

    UntypedNode::onrun<DrawCall>([](AnyNode* node, DrawCall *dc){  dc->run(); });
    
    TypedNode<DrawCall>::onadd<File>([](AnyNode*_this,AnyNode*node){ 
        
        // auto dc = _this->is_a<DrawCall>();
        // auto file = node->is_a<File>();
        // if (!dc || !file) return node;

        // auto model = dc->vbo.import(file);
        
        // dc->update();

        // return _this->UntypedNode::add(new Ptr<Model>(model));]

        return nullptr;

    });

    ////////// MODEL.HPP 

    UntypedNode::oncreate<Model>([](AnyNode* node, Model *model) { node->name = model->file->name; });

    TypedNode<Model>::onadd<File>([](AnyNode*_this,AnyNode*node){ 
        
        // auto model = _this->is_a<Model>();
        // auto file = node->is_a<File>();
        // if (!model || !file) return node;

        // auto bad = new ShaderFX(file); // unowned...

        // model->addFX(bad);

        // auto dc = _this->parent()->is_a<DrawCall>();
        // if (dc) dc->update();
        
        // return _this->UntypedNode::add(new Ptr<ShaderFX>(bad));
        

        return nullptr;

    });
    
    UntypedNode::oncreate<ShaderFX>([](AnyNode* node, ShaderFX *fx) { node->name = fx->file->name; });


    ////////// Directory.HPP 

    UntypedNode::oncreate<Directory>([](AnyNode* node, Directory *dir){ 

        node->name = dir->path;

        for (auto f : dir->list) node->addOwnr<File>(f);
        
    });

}