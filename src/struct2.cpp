#include "struct2.hpp"

namespace TEST {

        Member* TEST::Member::copy(Member* x = nullptr) { 

            if(!x) x = new Member(name_v); 
            
            x->name(name());

            x->striding(striding());

            x->quantity = quantity;    

            x->members = members;    

            for (auto &m : x->members) m = m->copy();
            
            return x; 
            
        }

};