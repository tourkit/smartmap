#pragma once

#include "gui.hpp"
#include "utils.hpp"
#include <map>
#include <functional>

struct Node;

struct EDITOR  {

    static inline std::map<TypeIndex, void*> ptr ;

};

template <typename T>
struct Editor  {

    static inline std::function<void(Node*,T*)> cb = nullptr;

    Editor(std::function<void(Node*,T*)> cb) { 
        
        Editor<T>::cb = cb; 

        EDITOR::ptr[typeid(T)] = &Editor<T>::cb;
        
    };

};

struct EditorWidget : GUI::Window {

    Node* selected = nullptr;

    bool locked = false;

    EditorWidget(GUI* gui);
    ~EditorWidget();

    void draw() override;

};
