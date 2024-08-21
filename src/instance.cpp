#include "instance.hpp"
#include "member.hpp"
#include "struct.hpp"
#include "buffer.hpp"
#include "src/utils.hpp"
#include <cctype>
#include <cstdint>
#include <format>
#include <string>
#include <vector>



std::pair<std::string,int> nameQ(std::string name) { 
    
    auto inbracket = name.find("[");

    int eq = 0;

    if (inbracket && name.back() == ']') {
        
        auto bracket = name.substr(inbracket+1,name.length()-4);

        bool all_digits = true;
        for (int i = 0; i < bracket.length(); i++) 
            if (!std::isdigit(bracket[i])) {
                all_digits = false;  
                break;
            }
        
        if (all_digits) 
            eq = std::stoi(bracket);
        else {PLOGE << "WEIRDASHIZE";}

        name = name.substr(0, inbracket);

    }
    
    return {name,eq}; 
        
}


    std::string Instance::stl_name() {

        std::vector<std::string> names;

        for (auto x : stl)  {

            auto name = x.m->name();
            if (x.eq>0) name += "[" + std::to_string(x.eq) + "]";
            names.push_back(name);
            
        }

        return join(names);

    }

    Instance& Instance::loc(int id, int eq) {

        if (stl.size() && id < stl.back().m->members.size()) {

            for (int i = 0; i < id; i++) 
                offset += stl[i].m->footprint_all();

            offset+=stl[id].m->footprint()*eq;

            stl.push_back({stl.back().m->members[id],eq});

            return *this;

        }

        PLOGE << id << " < " << stl.size();
        
        return *this;

    }

    Instance& Instance::loc(Member* m, int eq) {

        int offset = 0;

        if (stl.size()) 

            for (auto x : stl.back().m->members) {
        
                if (x == m) {

                    this->offset += offset+m->footprint()*eq;
                    stl.push_back({x,eq});
                    return *this;

                }else 
                    offset += x->footprint_all();
            }

        PLOGE << "couldn't find " << m->name();

        return *this;

    }

    Instance& Instance::loc(std::string stl_name) {

        auto names = split(stl_name);
        
        if (!names.size()) return *this;

        // if no stl find first in structs
        if (!stl.size()) {
            
            auto name = nameQ(names[0]);

            for (auto x : Member::structs) {

                if (x->name() == name.first) {

                    if (name.second > x->quantity()){

                        PLOGE << name.second << " > " << x->quantity();
                        return *this;

                    }
                
                    stl.push_back({x,name.second});
                    offset += x->footprint()*name.second;
                    names.erase(names.begin());
                    break;    

                }

            }

            if (!stl.size()) {
                PLOGE << " no " << name.first << " in structs";
                return *this;
            }
        
        }

        // then find each

        MemberQ *curr = &stl.back();

        while (names.size()) {

            Member* found = nullptr;

            auto name = nameQ(names[0]);

            for (auto &m : curr->m->members) 
            
                if (m->name() == name.first){ 

                    if (name.second < m->quantity()) {

                        if (names.size() >1 || name.second ) 
                            offset += m->footprint() * name.second;
                        
                        else curr->eq = name.second;

                    } else { PLOGE << name.second << " > " << m->quantity(); }

                    found = m;

                    break;

                }else

                    offset += m->footprint_all();
                

            if (!found) {

                PLOGE << "couldnt find: " << names[0];
                break;
                
            }

            names.erase(names.begin());

            stl.emplace_back(found,name.second);

            curr = &stl.back();

        }

        return *this;

    }

    Instance::Instance(std::string stl_name) { loc(stl_name); }

    Instance::Instance(Member& m) { stl.push_back({&m}); }

    Instance::Instance(const Instance& other) : stl(other.stl), offset(other.offset) { }

    int Instance::size() { return stl.front().m->footprint_all();}

    char* Instance::data() {

#ifdef ROCH
        if (!stl.size()) 
            { PLOGE << "ZUMBAKAFEW"; } // cause unsafe "prod" shouldnt be anywayz
#endif

        char* ptr = stl.front().m->data();

        return ptr ? ptr + offset : nullptr;

    }



    void Instance::set(void* ptr, size_t size) {

        memcpy(data(), ptr, size);

        for (auto &inst : stl.back().m->instances) {
        
            // for (auto r : inst.get()->remaps) 
            
            //     r->update(); 
        }

    }

    void Instance::calcOffset() {

        offset = 0;
            
        for (int i = 0; i < stl.size()-1; i++) {

            auto m = stl[i];

            offset += m.m->footprint()*m.eq;

        }

    }

    void Instance::setDefault(Member* m, int offset) {

        if (!m) m = stl.back().m;

        for (auto x : m->members) {

            setDefault(x, offset);

            offset += x->footprint_all();

        }

        if (m->def()) {

            for (int i = stl.size()-1; i>=0; i--) { 

                auto x = stl[i].m;

                if (x->quantity()>1) 
                    for (int todo = 0 ; todo < x->quantity(); todo++) {
                        memcpy(data()+(offset+x->size()*todo), m->def(), m->size());
                        // PLOGD << *(float*)m->def();
                    }

            }

            memcpy(data()+offset, m->def(), m->size());

        }

    }


Instance& Instance::track() {

    if (!stl.size()) return *this;

    auto m = stl.back().m;

    // if already such instance
    for (auto inst : m->instances) 
        if (inst->stl == stl) 
            return *inst;
    
    // else create new
    PLOGV << stl_name();

    m->instances.emplace_back(std::make_shared<Instance>(*this));

    return *m->instances.back().get();

}

int Instance::eq() {
    return stl.size()?stl.back().eq:0;
}
Instance& Instance::eq(int id) {

    auto &mq = stl.back();

    if (id >= mq.m->quantity()) {

        PLOGE << id << " > " << mq.m->quantity() << ")" ;
    
        return *this;
    
    }

    offset += mq.m->footprint()*(id-mq.eq);

    mq.eq = id;
    
    return *this;

}

void Instance::each(std::function<void(Instance&)> cb) { 

    auto x = stl.back().m;

    int offset= 0;

    for (auto m : x->members) {

        Instance inst(*this);

        inst.stl.push_back({m});

        inst.offset += offset;

        inst.each(cb);

        cb(inst);

        offset += m->footprint_all();

    }

}
