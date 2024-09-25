#include "instance.hpp"
#include "member.hpp"
#include "buffer.hpp"
#include "remap.hpp"
#include "src/utils.hpp"
#include <cctype>
#include <string>
#include <vector>


#include "member.hpp"




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


    std::string Instance::stl_name(int q) {

        std::vector<std::string> names;

        if (q)
            q = stl.size()-q-1;
        
        for (int i = q; i < stl.size(); i++) {

            auto name = stl[i].m->name();
            if (stl[i].eq>0) name += "[" + std::to_string(stl[i].eq) + "]";
            names.push_back(name);
            
        }

        return join(names);

    }

    void Instance::stlAdd(Member* m, int eq) {

        //add to stl
        stl.emplace_back(m,eq, m->quantity());

        // update instance
        if (m->instances.find(this) == m->instances.end()) {
            
            if (stl.size()>1)
                
                stl[stl.size()-2].m->instances.erase(this);
            
            // ADD_UNIQUE<Instance*>(m->instances, this);
            m->instances.insert(this);
            
        }

    }
    
    std::string Instance::print(bool label, Member* m, int offset, std::string prefix) {

        if (!m) m = stl.back().m;

        if (offset<0) offset = this->offset;

        std::string out;

        for (auto x : m->members) {

            for (int i = 0; i < x->quantity(); i++)
                out += print(label,x,offset+x->footprint()*i,prefix+x->name()+"::");


            offset += x->footprint_all();
        }

        if (m->isData()) {

            std::string val;
            if (m->type().id == typeid(float)) {
                
                val = std::to_string(*(float*)(data()+offset));

                int cut = 0;
                if (val.length()) {
                    
                    while (val[val.size()-1-cut] == '0') {
                        cut++;
                        if (val[val.size()-2-cut] == '.'){

                            cut+=2;
                            break;
                        }
                    
                    }

                    val = val.substr(0,val.length()-cut);
                
                }
            
            } else if (m->type().id == typeid(int)) val = std::to_string(*(int*)(data()+offset));
            
            else if (m->type().id == typeid(char)) val = std::to_string(*(char*)(data()+offset));

            if (label) out += prefix+m->name()+ " : ";
            
            out += val;
            
            if (label) out +=" \n";
            else out += ", ";
        
        }

        if (m->striding()) {
            
            if (label) 
                out += prefix + "stride : " + std::to_string(m->stride()/m->members[0]->type().size())+" \n";
            
            else
                for (int i = 0; i < m->stride()/m->members[0]->type().size(); i++) 
                    out += "0, ";
            
        }


        if (m == stl.back().m)  { PLOGW << (!label?"[ ":"\n") << (out.length()?out.substr(0,out.length()-2):out) << (!label?" ]":""); }

        return (out);

    }


    Instance::Instance(std::string stl_name) { loc(stl_name); }

    Instance::~Instance() { 

        for (auto r : remaps) {
            r->src = nullptr;
        } 
        stl.back().m->instances.erase(this); 
    }

    Instance::Instance(Member& m) { stlAdd(&m); }

    Instance::Instance(const Instance& other) 
        : stl(other.stl), offset(other.offset) { 
            
            stl.back().m->instances.insert(this); 
        }

    Instance& Instance::parent() {

        offset -= m()->footprint()*stl.back().eq;
        stl.pop_back();
        return *this;
    }

    Instance& Instance::loc(int id, int eq) {

        if (id < 0)
            return parent();

        if (!stl.size()) 
            return *this;

        auto m = stl.back().m;

        if (id < m->size()) {

            for (int i = 0; i < id; i++) 
                offset += stl[i].m->footprint_all();

            offset+=stl[id].m->footprint()*eq;
            
            stlAdd(m->members[id],eq);

            return *this;

        }

        PLOGE << id << " < " << stl.size();
        
        return *this;

    }

    Instance& Instance::loc(Member* m, int eq) {

        int offset = 0;

        if (stl.size()) {

            for (auto x : stl.back().m->members) {
        
                if (x == m) {

                    this->offset += offset+m->footprint()*eq;
                    stlAdd(x,eq);
                    return *this;

                }else 
                    offset += x->footprint_all();
            }

        }
        
        PLOGE << "couldn't find " << m->name();

        return *this;

    }
    
    Member* Instance::m() { if (stl.size()) return stl.back().m; return nullptr; }

    Member* Instance::buff() { if (stl.size()) return stl.front().m; return nullptr; }


void branchSTL(std::vector<MemberQ>& stl,  int pos=0) {
  
    for (int i = 0; i < stl[pos].m->quantity(); i++){

        if (pos <  stl.size()-1){
            std::cout << pos ;
            branchSTL(stl, pos+1);
       } else
            // if (pos==stl.size()-1)
                std::cout << pos << stl[pos].m->name() << "\n";
    }
}


    void Instance::post_change(std::vector<MemberQ> adding_list) {

        each([&](Instance& inst) {

            // instances update offset
            for (auto &x : inst.m()->instances) {

                if (x != &inst && x->stl.size() == inst.stl.size() && x->buff() == buff()) {

                    bool diff = false;
                    for (int i = 1; i < inst.stl.size(); i++) 
                        if (inst.stl[i].m != x->stl[i].m) {

                            diff = true;
                            break;

                        }

                    if (!diff) {
                        x->offset = inst.offset; 
                        for (auto e : x->stl) x->offset += e.m->footprint()* e.eq;
                        
                    }

                }

            }

            for (auto adding : adding_list) 
                if (adding.m == inst.m() && adding.m->quantity()){



                    // auto parent = inst.parent();

                    // while(parent) // do to them too
                    for (int i = 0; i < adding.q; i++)  {

                        inst.eq(adding.eq+i);
                        inst.setDefault();

                    }


                }

        });   

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
                
                    stlAdd(x,name.second);
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

            stlAdd(found,name.second);

            curr = &stl.back();

        }

        return *this;

    }

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
        buff()->upload();

    }

    void Instance::setDefault(Member* m, int offset) {

        if (!m) m = this->m();

        for (auto x : m->members) {

            for (int eq = 0 ; eq < m->quantity(); eq++) 

                setDefault(x, offset+m->footprint()*eq);
            
            offset += x->footprint_all();

        }

        char* mdef = m->def();

        if (m->def()) 

            for (int eq = 0 ; eq < m->quantity(); eq++) 

                memcpy(data()+(offset+m->footprint()*eq), m->def(), m->size());

                
            
    }



int Instance::eq() {
    return stl.size()?stl.back().eq:0;
}
Instance& Instance::eq(int id) {

    auto &mq = stl.back();

    if (id >= mq.m->quantity()) {

        PLOGE << id << " >= " << mq.m->quantity() ;
    
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

        inst.stlAdd(m);

        inst.offset += offset;

        inst.each(cb);

        cb(inst);

        offset += m->footprint_all();

    }

}