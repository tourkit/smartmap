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


struct Ref;

struct Struct : Member {

    Struct(std::string name = "", uint32_t quantity = 1);

    Struct(const Member& other);

    ~Struct();

    // Struct& add(Member& m, std::string name = "");

    template <typename T>
    Struct& add(std::string name = "", uint32_t quantity = 1) { auto n = new Data<T>(name, quantity); add(n); return *this; }

    // Struct* add(std::string name) ;

    Struct& range(float from, float to, float def) ;

    Struct& remove(Member& s) ;
    Struct& removeHard(Member& s) ;

    Struct& clear();

    void update() override;

    std::type_index type() override ;

    uint32_t size() override ;

    Member* copy() override ;

    void hard_delete();

    static inline std::set<Struct*> owned;

    static Struct& create(std::string name, uint32_t quantity = 1);

    static Struct* exist(std::string name);

    static Struct& id(std::string name);

    static bool destroy(std::string name) ;

    Struct* isStruct() override;

    Member* add(Member* m);
protected:
    static inline std::set<Member*> removing;

#ifdef ROCH
    std::string DEBUG_TYPE;
#endif


};


struct Ref : Struct {

    Ref(std::string name = "Ref", uint32_t quantity = 1) : Struct(name, quantity) { }

    Ref(const Member& other) : Struct(other) { }

    ~Ref() {  }

    Member* copy() override { return new Ref(*this); }

    bool isRef() override { return true; }

};
