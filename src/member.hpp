#pragma once

#include <cstdint>
#include <unordered_set>
#include <typeindex>
#include <functional>
#include <string>


#include "glm/glm.hpp"


static int nextFactor2(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }
static std::string camel(std::string str) { str[0] = std::toupper(str[0]); return str; }
static std::string lower(std::string str) { str[0] = std::tolower(str[0]); return str; }

struct Member {

    static inline std::unordered_set<Member*> pool; // has a ptr to all axisting Members;

    Member(std::string name_v = "") ;

    virtual ~Member();

    uint32_t quantity_v = 1;

    virtual void update() ;

    virtual void destroy() ;

    virtual uint32_t size();

    virtual uint32_t footprint();

    void quantity(uint32_t quantity_v) ;

    uint32_t quantity();

    uint32_t eq(int i) ;

    virtual uint32_t footprint_all();

    virtual std::type_index type() ;

    const char* type_name() ;

    uint8_t count() ;

    void name(std::string name_v) ;

    virtual std::string name();

    Member* any() ;

    void* range_from_ptr = nullptr;
    void* range_to_ptr = nullptr;
    void* default_val_ptr = nullptr;

    uint32_t stride();

    void striding(bool is_striding);

    bool striding();

    virtual Member* copy(Member* x = nullptr) ;

    virtual bool typed();

    std::vector<Member*> members;

    std::string print(int recurse = 0);

    void hard_delete();

    uint32_t size_v = 0;

protected:

    std::string name_v;

    Member* add(Member* m);

    Member* remove(Member* m);


private:

    bool is_striding = false;

};
