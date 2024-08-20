#pragma once

#include <string>
#include <set>

#include "member.hpp"


struct Struct : Member {

    /// static 

    static inline std::set<Struct*> owned;

    static Struct& create(std::string name, uint32_t quantity = 1);

    static Struct* exist(std::string name);

    static Struct& id(std::string name);

    static bool removeFromOwned(std::string name) ;


};

