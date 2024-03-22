#include "member.hpp"

Member::~Member() { 
        
            // remove from any Members in the pool
            for (auto m : pool) m->each([this](Member& m_) { m_.remove(this);  });
            
            // remove from pool
            pool.erase(this); 

            // delete typed() a.k.a Data members
            for (auto x : members) if (x->typed()) delete x;
            
            PLOGV << "~" << name(); 
            
    }
