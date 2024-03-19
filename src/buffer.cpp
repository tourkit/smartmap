
#include "buffer.hpp"
#include "instance.hpp"


Instance Buffer::operator[](const char* name) { return Instance{this,0,this}[name]; }

    //   void Buffer::remapEach(Buffer& from, Member* from_m, Member* to_m) {

    //     }
