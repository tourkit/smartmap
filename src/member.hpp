#pragma once

#include <cstdint>
#include <set>
#include <typeindex>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include "utils.hpp"




// struct Instance;

struct Member {

    static inline std::set<Member*> structs;

    static inline std::set<Member*> buffers;

    Member(std::string name);

    Member(std::string name, int quantity);

    Member(std::string name, TypeIndex type);

    Member(const Member& other) ;

    virtual ~Member();

    void name(std::string value) ;

    std::string next_name(std::string name);

    std::string name();

    TypeIndex type();

    std::string type_name() ;

    Member& add(Member* m);

    template <typename T>
    Member& add(std::string name) { 
        
        auto n = new Member(name, typeid(T)); 
        
        add(n); 
        
        return *this; 
        
    }

    Member& range(float from, float to, float def) ;

    bool remove(Member& s) ;

    bool removeHard(Member& s) ;

    bool clear();

    Member* quantity(uint32_t value) ;

    uint32_t quantity();

    uint32_t size();

    uint32_t footprint();

    uint32_t footprint_all();

    uint32_t stride();
    
    void striding(bool value);

    bool striding();

    uint32_t eq(int i) ;

    std::set<Member*> getTop(bool z = false);

    virtual void pre_change();

    virtual void update() ;

    struct NewMember{Member* m; int eq = 0, q = 1; };
    virtual void post_change(std::vector<NewMember> added = {});

    bool each(std::function<bool(Member*)> cb) ;

    bool each(std::function<bool(Member*,int)> cb, int offset = 0);

    virtual Member* copy();

    bool isData();
    
    void deleteData();

    Member* ref();
    bool ref(Member*);

    // INSTANCEMGMT
    // std::vector<std::shared_ptr<Instance>> instances;

    bool is_copy = false;

    // void 

    // template <typename T>
    // struct TypedMember : Member {

    //     using Member::Member;

    //     T& from() { return *(T*)from(); }
    //     T& to() { return *(T*)to(); }
    //     T& def() { return *(T*)def(); }
    // };
    
    //// DATA TRUCS
    
    template <typename T>
    void type() { type(typeid(T)); }

    void type(TypeIndex type);

    char* from();
    char* to();
    char* def();
    

    //// BUFFER TRUCS

    void remap(Member& src_buffer, Member* src_member = nullptr, Member* this_member = nullptr, int src_offset = 0, int this_offset = 0);

    virtual void upload();



    bool isBuff();
    bool buffering();
    void buffering(bool value);

protected:

    std::string name_v;

#ifdef ROCH
    std::string DEBUG_TYPE = "undefined";
#endif

    uint32_t size_v = 0;

    int32_t quantity_v = 1;
    
    bool striding_v = false;

    std::vector<char> buffer_v;

    std::vector<char> rangedef; 

    TypeIndex type_v = typeid(Member);

    static inline std::set<Member*> removing;

    static inline int MAX_SIZE = 10000;

    bool is_buffer = false;
    
    Member* ref_v = nullptr;

private:

    Member* bkp = nullptr;


public:

    std::vector<Member*> members;

};
