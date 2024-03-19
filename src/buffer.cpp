
#include "buffer.hpp"
#include "instance.hpp"


Instance Buffer::operator[](std::string name) { return Instance{this,0,this}[name]; }
Instance Buffer::operator[](int id) { return Instance{this,0,this}[id]; }

    //   void Buffer::remapEach(Buffer& from, Member* from_m, Member* to_m) {

    //     }
