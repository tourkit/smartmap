#include "instance2.hpp"
#include "struct2.hpp"
#include "buffer2.hpp"

   bool TEST::Instance::exist(){ 

            if (offset == -1) return false;

            return true;

        }

        TEST::Instance TEST::Instance::operator[](const char* name) { 


            int offset = 0;

            AnyMember* found = nullptr;
            PLOGD << "find " << name << " in " << (found?found->name():"buff");
            if (!member) member = buffer;
            int yy = 0;
            for (auto &m : member->members) { 

                PLOGD << m.get()->name()<<"_"<<m.get()->footprint();

                if (!(strcmp(m.get()->name().c_str(),name))) {
                
                    found = m.get();
                    
                    PLOGD << m.get()->name().c_str() <<yy<< " is " << name<<offset;

                    break;
                
                }

                
                offset += m.get()->footprint();
                
            }
            if (!found) offset = -1;

            return Instance{buffer,this->offset+offset,found};

        }


