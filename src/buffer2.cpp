
#include "buffer2.hpp"
#include "instance2.hpp"

namespace TEST {

Instance Buffer::operator[](const char* name) { return Instance{data.data(),0,this}[name]; }

    //   void Buffer::remapEach(Buffer& from, Member* from_m, Member* to_m) {

    //     }

        void Buffer::remap(Buffer& from) {

            data.resize(from.data.size());
            
            memset(data.data(),0,data.size());

            PLOGW <<"la"<< &from.data;

            remapEach(from);

            PLOGW <<"la"<< &from.data;
        }        


};