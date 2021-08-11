#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include "RectangleBinPack/GuillotineBinPack.h" 
#include "test.hpp"

struct Attribute {

  static inline std::vector<float> attributes;

  unsigned int id; // can be address x.y if attributes is vec of vec

  float cur_val;

  std::set<Attribute*> links_to;

  Attribute* link_from;

  Attribute(float v) : Attribute() { set(v); }
  Attribute(int v) : Attribute() { set((float)v); }
  Attribute() {

    cur_val = 0;

    link_from = nullptr;

    id = attributes.size();

    attributes.push_back(0);

  }

  ~Attribute() {

    if (link_from) link_from->unlink(this);

    for (auto* l:links_to) unlink(l);

  }

  void link(Attribute* dst) {

    links_to.insert(dst);

    dst->link_from = this;

    dst->update(attributes[id]);

  }

  void unlink(Attribute* dst) {

    links_to.erase(dst);

    dst->link_from = nullptr;

    dst->update(dst->cur_val);

  }

  void set(const float& val) {

    cur_val = val;

    update(val);

  }

  void update(const float& val) {

    attributes[id] = val;//std::min(std::max(val,min_val),max_val);

    for (auto* l:links_to) attributes[l->id] = attributes[id];

    // + callback() ?

  }

  const float& get() { return attributes[id]; }

  operator float() { return get(); }

  void operator= (const float& val) {  set(val); }
  void operator= (const int& val) {  set((float)val); }

};

struct AdvancedAttribute : public Attribute {

  float min_val, max_val, def_val;

  void min() { set(min_val); }
  void min(const float& v) { min_val = v; }
  void max() { set(max_val); }
  void max(const float& v) { max_val = v; }
  void def() { set(def_val); }
  void def(const float& v) { def_val = v; }

  // GUI STUFF HERE ?

};

struct Buffer {

  static inline std::vector<Buffer*> pool;

  Attribute width, height;

  std::string name;

  // Coords size, pos;

  unsigned int dc_id;

  Buffer()  { pool.push_back(this); }
  Buffer(Attribute width, Attribute height) : Buffer() { size(width, height); }
  void size(Attribute width, Attribute height)  {

  this->width = width;
  this->height = height;
}

};


struct Renderer;

struct Drawcall {

  static inline std::map<int, std::vector<Drawcall>> stack;

  unsigned int id, width, height;

  Buffer* dst ;
  std::set<Buffer*> sources;

  Buffer buffer;

  rbp::GuillotineBinPack atlas;

  using Rect = rbp::Rect;

  Drawcall() : atlas(16000,16000) { }

  void build() { std::vector<Rect> matrice = atlas.GetUsedRectangles(); }

  int add(Renderer* src, Buffer* dst);

  static void add(Renderer* src, Buffer* dst, int level) {

     if (stack[level].size()<1) stack[level].resize(1);

     if (!stack[level].back().add(src,dst)) {
    
       stack[level].resize(stack[level].size()+1);

       add(src,dst,level);

     }

   }

};


struct Renderer {

  static inline std::vector<Renderer*> pool;

  std::string name;

  Renderer() { pool.push_back(this); }

  virtual void dc(Buffer* dst = 0) = 0;

};

struct Obj : public Renderer {

    Buffer* texture;

    Obj() { name = "Obj"; }

    void dc(Buffer* dst) override {

      Drawcall::add(this, dst, (texture==dst));

    }

};

struct Compo : public Obj {

  std::vector<Obj*> tree;

  Buffer buffer;

  Compo(Attribute width, Attribute height) : buffer(width,height) {

    name = "Compo"+std::to_string(pool.size());
    buffer.name = "Buffer_"+name;


  }

  void dc(Buffer* dst = 0) override {

    for (Renderer* o : tree) { o->dc(&buffer); }

    Obj::dc(dst); 

  }

}; 

struct Pass : public Compo {

  Pass(Compo* parent) : Compo(parent->buffer.width, parent->buffer.height) { name = "Pass"; texture = &parent->buffer; }

  void dc(Buffer* dst = 0) override { Compo::dc(dst); }

};

struct Output : public Renderer {

  unsigned int width, height;

  std::vector<Obj*> tree;

  Output(unsigned int width, unsigned int height) : width(width), height(height) { name = "Output"; }

  void dc(Buffer* dst = 0) override {


  }

};

int Drawcall::add(Renderer* src, Buffer* dst) { 

  auto matrice = atlas.GetFreeRectangles(); 

  for (auto cell : matrice )  {

    if (dst->width < cell.width && dst->height < cell.height) {

        atlas.Insert(
          
          dst->width, dst->height,
          0,
          rbp::GuillotineBinPack::RectBestAreaFit, 
          rbp::GuillotineBinPack::SplitShorterAxis
        
        );

        sources.insert(dst);

        return 1;

        break;

    }

  }

  return 0;

    // std::cout << "Stack::addRenderer(" << src->name << ", " << dst->name  << ")\n";

}

int main() {

  Buffer zero(1920,1200); zero.name = "Zero";
  std::vector<Renderer*> tree;

  // tree.push_back(new Compo(800,600));
  // Compo* output = (Compo*)tree[0];
  // Compo* bbb = new Compo(100,100);
  // // bbb->name="koubi";

  // output->tree.push_back(new Pass(output));
  // output->tree.push_back(new Obj);
  tree.push_back(new Obj);

  for (auto* r : tree) r->dc(&zero);

  Drawcall dc;

  std::cout <<"\n\n";

}
