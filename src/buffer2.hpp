#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>

#include "struct2.hpp"




namespace TEST {

    struct Instance;

    struct Buffer : Struct{

        std::vector<char> data;
        
        Buffer() : Struct("buffer_definition") {

            data.reserve(1000);
            memset(data.data(),0,data.size());

        }

        void set(Member& m, void* data) { }

        void update() override { data.resize( footprint_all() ); Struct::update(); }

        Instance operator[](const char* name);

        Member* copy(Member* x) override { 
            
            if (!x) x = new Buffer(); 

            Struct::copy(x);

            ((Buffer*)x)->data = data;

            return x; 
            
        }
        
        Buffer& copy() { return *(Buffer*)copy(nullptr); }

        void remapEach(Buffer& from, Member* from_m = nullptr, Member* to_m = nullptr, int from_offset = 0, int to_offset = 0) {

            if (!from_m) from_m = &from;

            if (!to_m) to_m = this;

            int from_offset_curr = 0;

            for (int i = 0 ; i < from_m->quantity ; i ++) {
                
                for (auto from_m_curr : from_m->members) {

                    Member* found =nullptr;

                    int to_offset_curr = 0;

                    for (auto to_m_curr : to_m->members) {
                    
                        if (!strcmp(from_m_curr->name().c_str(), to_m_curr->name().c_str())) { found = to_m_curr; break; }
                    
                        else to_offset_curr+=to_m_curr->size();
                    
                    }
                    
                    if (!found) { PLOGW << "nofiund " << from_m_curr->name(); continue; }
                    if (found->typed()) {
                        
                        PLOGD << from_m->name() << " change : " << found->name() << " " <<(from_m->size()*i)+from_offset+from_offset_curr << " to " <<(to_m->size()*i)+to_offset+to_offset_curr << to_m->name();// << " from : " << lm.g
                        
                    }

                    remapEach(from, from_m_curr, found, from_offset+from_offset_curr, (to_m->size()*i)+to_offset+to_offset_curr);
                
                    from_offset_curr += from_m_curr->size();

                }
                
            }

        }

        void remapEach2(Buffer& buffer, Member* from_m = nullptr, Member* to_m = nullptr, int from_offset = 0, int to_offset = 0) {

            if (!from_m) from_m = &buffer;

            for (auto m : from_m->members) {
                
                remapEach2(buffer,m);

            }

            PLOGD << from_m->name();
        
        }

        void remap(Buffer& from); 

    };

    




};
