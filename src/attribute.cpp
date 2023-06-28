#include "attribute.hpp"  
	


Attribute::Attribute(std::string name, float val, float min_val, float max_val, std::vector<float>* dst) {  }

float* Attribute::ptr() { if (id <0) return child(0)->ptr(); else return &(*buffer)[id]; }

void Attribute::gui() {

    int size = childrens.size();
    if (!size) size = 1;

    if (size == 1) ImGui::SliderFloat(name.c_str(),ptr(),min_val,max_val); 
    else if (size == 2) ImGui::SliderFloat2(name.c_str(),ptr(),child(0)->min_val,child(0)->max_val); 
    else if (size == 3) ImGui::SliderFloat3(name.c_str(),ptr(),child(0)->min_val,child(0)->max_val); 
    else if (size == 4) ImGui::SliderFloat4(name.c_str(),ptr(),child(0)->min_val,child(0)->max_val); 

}

Attribute::Attribute(std::string name, std::vector<Attribute*> childrens, float min_val, float max_val) : name(name) { for (auto c:childrens) this->childrens.push_back(c); }

Attribute::Attribute(std::vector<Attribute*> childrens) : Attribute("", childrens) { }

float Attribute::get() { return (*buffer)[id]; }

// virtual void Attribute::set(float val) { (*buffer)[id] = val;  for (auto l:links_dst) l->set(val); } 

void Attribute::setNormalized(float val) { set(val*(range_val)+min_val); } 

Attribute* Attribute::child(int id) { return childrens[id]; }

Attribute* Attribute::child(std::string name) { 

    Attribute* output = this;

    char* t_attr = strdup(name.c_str());

    char * pch = strtok (t_attr, " ,.-");

    while (pch != NULL) {

        Attribute* found = nullptr;

        for (auto c:childrens) { 

            if (!strcmp(pch,c->name.c_str())) {

                found = c; 
                break;

            } 

        } 

        if (!found) std::cout << "\"" << pch << "\"" << " not found !" << std::endl; 
        else output = found;

        pch = strtok (NULL, ",.-");

    }

    free(t_attr);

    if (output == this) return nullptr;
    else return output; 

}

void Attribute::linkFrom(Attribute* src) {

src->links_dst.push_back(this);

link_src = src;

}

void Attribute::linkTo(Attribute* dst) { 
    dst->linkFrom(this); 
    }

Attribute::~Attribute() { for (auto l:links_dst) l->link_src = nullptr; }