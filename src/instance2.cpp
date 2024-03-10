#include "instance2.hpp"
#include "struct2.hpp"
#include "buffer2.hpp"

   bool TEST::Instance::exist(){ 

            if (offset == -1) return false;

            return true;

        }

        TEST::Instance TEST::Instance::operator[](const char* name) { 

            if  (offset < 0) return Instance{data,offset-1,member};

            AnyMember* found = nullptr;

            if (!member) {PLOGW << "BUGGY";exit(0);}

            for (auto &m : member->members) { 

                if (!(strcmp(m->name().c_str(),name))) {
                
                    found = m;

                    break;
                
                }
                
                offset += m->footprint()*m->quantity;
                
            }

            if (!found) { offset = -1; PLOGW << "\"" << name << "\" does not exist"; }
        
            return Instance{data,offset,found};

        }
TEST::Instance& TEST::Instance::eq(int id) {

        if (!member || id >= member->quantity) return *this;
        
        id = id-this->id;

        offset += member->footprint() * id;
        
        return *this;

}