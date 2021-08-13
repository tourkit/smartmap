#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include "include/vendor/RectangleBinPack/GuillotineBinPack.h" 
#include "test.hpp"

using Rect = rbp::Rect;


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

  std::vector<char> data;

  bool CPP_STORE_DATA = 0;

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

struct Call {

  static inline std::vector<Call*> stack;

  int active;

  Call() { active = true; }

  virtual void routine() { };

};

struct Drawcall : public Call {

  unsigned int width, height;

  std::set<Buffer*> sources;

  Buffer* dst;

  static inline void Atlasify() { /* for (auto* dc:pool) { Atlas::add(dc); } */ }

};

std::vector<Call> stack;

struct Atlas : public Drawcall {
    
  static inline std::vector<Atlas*> pool;

  Atlas() : binpack(16000,16000), buffer(16000,16000) {  }

  rbp::GuillotineBinPack binpack;
  
  Buffer buffer;

  void add(Drawcall* dc){ 

    

    //  if (stack[level].size()<1) stack[level].resize(1);

    //  if (!stack[level].back().add(src,dst)) {
    
    //    stack[level].resize(stack[level].size()+1);

    //    add(src,dst,level);

    //  }

   

    if(sources.find(dc->dst) != sources.end()) { /* need to be in other atlas deeper level */ }

    else { 

      for (auto cell : binpack.GetFreeRectangles() )  {

        if (dc->width < cell.width && dc->height < cell.height) {

            binpack.Insert(
              
              dc->width, dc->height,
              0,
              rbp::GuillotineBinPack::RectBestAreaFit, 
              rbp::GuillotineBinPack::SplitShorterAxis
            
            );

            for (auto& s:dc->sources) sources.insert(s); 
            
            // std::cout << "Stack::addRenderer(" << src->name << ", " << dst->name  << ")\n";
            
            break;

        }

      }

      /* COULD NOT FIT IN BINPACK : need to be in other atlas same level */ 

      pool.push_back(new Atlas); 

      add(dc);


    }

  }

};

struct Renderer : public Drawcall {

  static inline std::vector<Renderer*> pool;

  std::string name;

  Buffer* dst;

  Renderer() { pool.push_back(this); }

  virtual void dc(Buffer* dst = 0) = 0;

};

struct Obj : public Renderer {

    Buffer* texture;

    Obj() { name = "Obj"; }

    void dc(Buffer* dst) override {

      // auto* dc = Drawcall::add(this, dst, (texture==dst)); 

      // dc->sources.push_back(Stack::adress[texture]);
      
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





int main() {

  Buffer zero(1920,1200); zero.name = "Zero";
  std::vector<Renderer*> tree;

  tree.push_back(new Compo(800,600));
  Compo* output = (Compo*)tree[0];
  Compo* bbb = new Compo(100,100);
  // bbb->name="koubi";

  output->tree.push_back(new Pass(output));
  output->tree.push_back(new Obj);
  tree.push_back(new Obj);

  for (auto* r : tree) r->dc(&zero);

  std::cout <<"\n\n";

    int count = 0;

    for (auto& dcs : Drawcall::stack) {

    //   for (auto& dc : dcs.second) {

    //     for (auto y : dc.atlas.GetUsedRectangles() )  
        
    //         std::cout << "id:" << dcs.first << " size(" << y.width 
    //         << "/" << y.height << ") offset(" << y.x << ", " 
    //         << y.y <<  ")\n";

    //         // comment un BFR connait sa stack address ?

    //         // une map [buffer->id, Rect] 

    //   }

    }


struct Test {
    
    Test() {  }
}


}
