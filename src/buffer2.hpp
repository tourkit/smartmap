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

        void update() override { 
            Struct::update(); 
            data.resize( footprint_all() ); 
            
            
            memset(data.data(),0,data.size());

        }

        Instance operator[](const char* name);

        Member* copy(Member* x) override { 
            
            if (!x) {x = new Buffer();} 

            Struct::copy(x);

            ((Buffer*)x)->data = data;
    
            return x; 
            
        }
        
        Buffer& copy() { 
            Buffer* x= (Buffer*)Buffer::copy(nullptr); 
            Buffer& y= *x; 
            Buffer*z= &y; 
            return y;
        }

        void remapEach(Buffer& from, Member* from_m = nullptr, Member* to_m = nullptr, int from_offset = 0, int to_offset = 0) {

            if (!from_m) from_m = &from;

            if (!to_m) to_m = this;

            int from_offset_curr = 0;

            auto quantity = from_m->quantity;

            if (quantity > to_m->quantity) quantity = to_m->quantity;

            for (int i = 0 ; i < quantity; i ++) {
                
                for (auto from_m_curr : from_m->members) {

                    Member* found = nullptr;

                    int to_offset_curr = 0;

                    for (auto to_m_curr : to_m->members) {
                    
                        if (!strcmp(from_m_curr->name().c_str(), to_m_curr->name().c_str())) { found = to_m_curr; break; }
                    
                        else to_offset_curr += to_m_curr->size();
                    
                    }
                    
                    if (!found) { PLOGW << "couldnt find " << from_m_curr->name(); continue; }

                    if (found->typed()) {

                        to_offset_curr += ( to_m->size() * i ) + to_offset;

                        from_offset_curr += from_offset;
                        
                        PLOGV << from_m->name() << " change : " << found->name() << " " <<(from_m->size()*i)+from_offset_curr << " to " <<to_offset_curr << to_m->name() << " (" << to_m->size() << ") = " << (unsigned int)data[to_offset_curr];
                     
                        memcpy(&data[to_offset_curr], &from.data[(from_m->size()*i)+from_offset_curr],to_m->size()); // dst ? this !
                        
                    }

                    remapEach(from, from_m_curr, found, from_offset_curr, to_offset_curr);
                
                    from_offset_curr += from_m_curr->size();

                }
                
            }

        }

        void remap(Buffer& from); 

    };

    




};
