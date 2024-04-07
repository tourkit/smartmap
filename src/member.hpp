#pragma once

#include <cstdint>
#include <set>
#include <typeindex>
#include <functional>
#include <string>


#include "glm/glm.hpp"


static int nextFactor2(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }
static std::string camel(std::string str) { str[0] = std::toupper(str[0]); return str; }
static std::string lower(std::string str) { str[0] = std::tolower(str[0]); return str; }

struct Struct;

struct Member {

    static inline std::set<Struct*> structs;

    Member(std::string name_v = "") ;

    virtual ~Member();

    virtual void update() ;

    virtual uint32_t size();

    uint32_t footprint();

    uint32_t footprint_all();

    void quantity(uint32_t quantity_v) ;

    uint32_t quantity();

    uint32_t eq(int i) ;

    uint32_t stride();

    void striding(bool is_striding);

    bool striding();

    virtual std::type_index type();

    std::string type_name() ;

    uint8_t count() ;

    void name(std::string name_v) ;

    std::string name();

    virtual Member* copy(Member* x = nullptr) ;

    virtual bool typed();

    virtual std::string print(int recurse = 0);

    virtual void hard_delete() {}

    std::vector<Member*> members;

    void* range_from_ptr = nullptr;
    void* range_to_ptr = nullptr;
    void* default_val_ptr = nullptr;

protected:

    std::string name_v;

    uint32_t quantity_v = 1;

    bool is_striding = false;


};
