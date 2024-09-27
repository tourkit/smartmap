#pragma once

#include "gui.hpp"
#include "utils.hpp"
#include <map>
#include <set>
#include <functional>

struct Node;

struct EDITOR  {

    static inline std::map<Node*, std::function<void(Node*)>> cb ;

    static inline std::map<TypeIndex, void*> ptr ;
    static inline std::set<Node*> triglist;

};

template <typename T>
struct Editor  {

    static inline std::function<void(Node*,T*)> cb_typed = nullptr;

    Editor(std::function<void(Node*,T*)> cb) { 
        
        Editor<T>::cb_typed = cb; 

        EDITOR::ptr[typeid(T)] = &Editor<T>::cb_typed;
        
    };

};

struct EditorWidget : GUI::Window {

    Node* selected = nullptr;

    bool locked = false;

    EditorWidget(GUI* gui, Node* selected = nullptr);
    ~EditorWidget();

    void draw() override;

};
