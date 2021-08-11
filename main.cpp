#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include "RectangleBinPack/GuillotineBinPack.h" 

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

struct Coords { float x,y; };

struct QuadCoords { Coords size, pos; };

std::vector<QuadCoords> matrice(unsigned int x, unsigned int y) {

    std::vector<QuadCoords> mat;

    float xsize = 1. / x;
    float ysize = 1. / y;

    for (size_t row = 0; row < x; ++row) for (size_t col = 0; col < y; ++col) mat.push_back({xsize, ysize, xsize * row,  ysize * col});

    return mat;

}

std::vector<QuadCoords> matrice(unsigned int q) {

    unsigned int m = std::ceil(std::sqrt(q > 1 ? q-1 : q ));

    auto mat = matrice(m,m);

    mat.resize(q);

    return mat;

}

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

struct Drawcall {

  static inline unsigned int uid = 0;

  unsigned int id, width, height;

  std::set<Buffer*> buffers;

  Buffer atlas;

  Drawcall() { id = uid++; }

  void binPack() {

    std::vector<QuadCoords> atlas = matrice(buffers.size());

    int i = 0;

    for (auto* b : buffers) {

      b->dc_id = id;

      // b->pos = atlas[i].pos;

      i++;


    }

  }

  void addBuffer(Buffer* buffer) {

      buffers.insert(buffer);
      binPack();

  }

};

struct Renderer;
struct Stack {

  static inline std::map<Buffer*,Drawcall> stack;
  static inline std::vector<Buffer*> passes;

  static void addRenderer(Renderer* src, Buffer* dst);

  static Buffer* createPass() { passes.push_back(new Buffer); return passes.back(); }

};


struct Renderer {

  static inline std::vector<Renderer*> pool;

  std::string name;

  Renderer() { pool.push_back(this); }

  virtual void dc(Buffer* dst, unsigned int level = 0) = 0;

};

struct Obj : public Renderer {

    Buffer* texture;

    Obj() { name = "Obj"; }

    void dc(Buffer* dst, unsigned int level = 0) override {

      Stack::addRenderer(this,dst);

    }

};

struct Compo : public Obj {

  std::vector<Obj*> tree;

  Buffer buffer;

  Compo(Attribute width, Attribute height) : buffer(width,height) {

    name = "Compo"+std::to_string(pool.size());
    buffer.name = "Buffer_"+name;


  }

  void dc(Buffer* dst, unsigned int level = 0) override {

    for (Renderer* o : tree) { o->dc(&buffer,level); }

    if (texture != dst) { Obj::dc(dst,level); }
    
    else {

      Buffer* pass = Stack::createPass();
      pass->name = "Buffer_Pass";
      Obj::dc(pass, level+1); //  glsubcopystuff() ify ?

    }

  }

};

struct Pass : public Compo {

  Pass(Compo* parent) : Compo(parent->buffer.width, parent->buffer.height) { name = "Pass"; texture = &parent->buffer; }

  void dc(Buffer* dst, unsigned int level = 0) override { Compo::dc(dst); }

};

struct Output : public Renderer {

  unsigned int width, height;

  std::vector<Obj*> tree;

  Output(unsigned int width, unsigned int height) : width(width), height(height) { name = "Output"; }

  void dc(Buffer* dst, unsigned int level = 0) override {


  }

};


void Stack::addRenderer(Renderer* src, Buffer* dst) {


    std::cout << "Stack::addRenderer(" << src->name << ", " << dst->name  << ")\n";

}


int main() {

  Buffer zero(1920,1200); zero.name = "Zero";
  std::vector<Renderer*> tree;

  tree.push_back(new Compo(800,600));
  Compo* output = (Compo*)tree[0];
  Compo* bbb = new Compo(100,100);
  // bbb->name="koubi";

  output->tree.push_back(new Pass(output));
  output->tree.push_back(new Obj);

  for (auto* r : tree) r->dc(&zero);

  std::cout <<"\n\n";

  Drawcall dc;
  
  Buffer bf1(100,100);

  dc.addBuffer(&bf1);


  rbp::GuillotineBinPack testbinpack(16000,16000);

  auto rectChoice = rbp::GuillotineBinPack::RectBestAreaFit;
  auto splitMethod = rbp::GuillotineBinPack::SplitShorterAxis;

  testbinpack.Insert(1920,1200,0,rectChoice,splitMethod);
  testbinpack.Insert(1920,1200,0,rectChoice,splitMethod);
  testbinpack.Insert(100,100,0,rectChoice,splitMethod);
  testbinpack.Insert(100,100,0,rectChoice,splitMethod);

  auto matrice = testbinpack.GetUsedRectangles(); 

  int count = 0;
  for (auto y : matrice )  std::cout << "id:" <<count++ << " size(" << y.width << "/" << y.height << ") offset(" << y.x << ", " << y.y <<  ")\n";

}
