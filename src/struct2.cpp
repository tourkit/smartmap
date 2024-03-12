#include "struct2.hpp"


namespace TEST {

        virtual Bkp* Member::copy(Member* x = nullptr) { 

            if(!x) x = new Bkp<Member>(); 
            
            x->name(name());

            x->striding(striding());

            x->members = members;
            
            return x; 
            
        }


};