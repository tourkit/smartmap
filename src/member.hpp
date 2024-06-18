#pragma once

#include <cstdint>
#include <set>
#include <typeindex>
#include <functional>
#include <string>
#include <vector>
#include <memory>


#include "glm/glm.hpp"
#include "log.hpp"



static int nextFactor2(int x, int factor = 4) { return ((int)((x-1)/(float)factor)+1)*factor*!!x; }
static std::string camel(std::string str) { str[0] = std::toupper(str[0]); return str; }
static std::string lower(std::string str) { str[0] = std::tolower(str[0]); return str; }

struct Struct;
struct Instance;
struct Buffer;
struct Member;


struct Member {

    static inline std::set<Struct*> structs;

    Member(std::string name_v = "") ;

    Member(const Member& other) ;

    virtual ~Member();

    virtual void update() ;

    virtual uint32_t size();

    uint32_t footprint();

    uint32_t footprint_all();

    Member* quantity(uint32_t quantity_v) ;

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

    virtual Member* copy();

    virtual bool isData();
    virtual bool isRef();
    virtual Struct* isStruct();
    virtual Buffer* isBuff();

    virtual void hard_delete() {}

    void each(std::function<void(Instance&)> f, Buffer* buff = nullptr,  uint32_t offset= 0, std::vector<Member*> stl = {});

    std::vector<Member*> members;

    std::vector<std::shared_ptr<Instance>> instances;

    void* range_from_ptr = nullptr;
    void* range_to_ptr = nullptr;
    void* default_val_ptr = nullptr;

    std::set<Member*> getTop(bool z = false);

    virtual void pre_change();

    struct NewMember{Member* m; int eq = 0, q = 1; };

    virtual void post_change(std::vector<NewMember> added = {});

    template <typename U>
    std::array<U,3> range() { return { *(U*)range_from_ptr, *(U*)range_to_ptr, *(U*)default_val_ptr}; }

    int32_t quantity_v = 1;

    std::string next_name(std::string name);

    int get_offset();

protected:

    std::string name_v;


    bool is_striding = false;

    bool is_copy = false;

    uint32_t size_v = 0;

};
