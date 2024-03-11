#include "struct2.hpp"


namespace TEST {

        virtual Bkp* AnyMember::copy(AnyMember* x = nullptr) { 

            if(!x) x = new Bkp<AnyMember>(); 
            
            x->name(name());

            x->striding(striding());

            x->members = members;
            
            return x; 
            
        }


};