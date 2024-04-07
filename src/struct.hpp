#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstddef>
#include <map>
#include <memory>
#include <unordered_set>
#include <set>

#include "member.hpp"
#include "data.hpp"
#include "log.hpp"



    struct Struct : Member {

        Struct(std::string name = "", uint32_t quantity = 1);

        static inline std::set<Struct*> owned;

        static Struct& create(std::string name, uint32_t quantity = 1);

        static Struct* exist(std::string name);

        static Struct& id(std::string name);

        static void clear();

        static bool destroy(std::string name) ;

        Struct& add(Struct& s);

        Struct& add(const char* name) ;

        Struct& remove(Struct& s) ;

        template <typename T>
        Struct& add(std::string name = "") { Member::add(new Data<T>(name)); return *this; }

        Struct& add(Member* m) ;

        Struct& range(float from, float to, float def) ;

        std::type_index type() override ;

        Member* copy(Member* x = nullptr) override ;

    };
