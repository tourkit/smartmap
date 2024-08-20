/*

   SMARTMAP ( v 1.3.1.2 beta ) 

                                */



#include "src/member.hpp"
#include "src/log.hpp"

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

struct Instance {

    struct MemberQ {

        Member* m;
        int eq = 0;
    
    };

    std::vector<MemberQ> stl;

    uint32_t offset = 0;


    std::string stl_name() {

        std::vector<std::string> names;

        for (auto x : stl)  {

            auto name = x.m->name();
            if (x.eq>0) name += "[" + std::to_string(x.eq) + "]";
            names.push_back(name);
            
        }

        return join(names);

    }

    void find(std::string stl_name) {

        auto names = split(stl_name);
        
        if (!names.size()) return;

        // if no stl find first in structs
        if (!stl.size()) {
            
            auto name = nameQ(names[0]);

            for (auto x : Member::structs) {

                if (x->name() == name.first) {

                    if (name.second > x->quantity()){

                        PLOGE << name.second << " > " << x->quantity();
                        return;

                    }
                
                    stl.push_back({x});
                    offset += x->footprint()*name.second;
                    names.erase(names.begin());
                    break;    

                }

            }

            if (!stl.size()) {
                PLOGE << " no" << name.first << " in structs";
                return;
            }
        
        }

        // then find each

        MemberQ &curr = *stl.begin();

        while (names.size()) {

            Member* found = nullptr;

            auto name = nameQ(names[0]);

            curr.eq = 0;

            for (auto &m : curr.m->members) 
            
                if (m->name() == name.first){ 

                    if (name.second < m->quantity()) {

                        if (names.size() >1 ) 
                            offset += m->footprint() * name.second;
                        
                        else curr.eq = name.second;

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

            stl.push_back({found,name.second});

            curr = stl.back();

        }

    }

    Instance(std::string stl_name) {

        find(stl_name);
    
    }

    Instance(Member& m) {

        stl.push_back({&m});
    
    }

};

int main() {

    // track could have owner list

    // inst.print()


    logger.cout(Sev::verbose);

    Member testbuf("testbuf");
    // testbuf.buffering(true);

    Member rgb("RGB");
    rgb.add<float>("red").range(0,1,1).add<float>("green").range(0,1,2).add<float>("blue").range(0,1,3);

    Member didoo("didoo");
    didoo.add<float>("didi").range(8,8,8).add<float>("dodo").range(9,9,9).striding(true);
    
    Member sa("Sa");
    sa.add(&rgb);

    testbuf.add(&sa);
    sa.quantity(2);

    std::string uno = "testbuf::Sa[1]::RGB::blue";
    std::string dos = "Sa[1]::RGB::blue";
    Instance inst(uno);

    Instance testbuf_(testbuf);
    testbuf_.find(dos);

    // Instance testbuf_(testbuf);
    PLOGD << inst.offset;
    PLOGD << testbuf_.offset;

    // isnt.set<float>(123);


    // auto &sa_ = testbuf.add(&sa).track();
    
    // Member sx("Sx");
    // sx.add(&didoo);
    // auto &sx_ = testbuf.add(&sx).track();
    
    // Member sb("Sb");
    // sb.add(&rgb);
    // auto &sb_ = testbuf.add(&sb).track();

    // sb_.set<float,3>(6.0f,66.0f,666.0f);

    // sx.quantity(2);

    // sa.quantity(10);
    
    // Instance("testbuf::Sb::RGB::red").remap("testbuf::Sa[2]::RGB::green");

    // for (int i = 0; i < 10; i++) 
    //     sa_.eq(i).set<float,3>(i*1.0f,i*2.0f,i*3.0f);

    // PLOGW << Instance(&testbuf).get<float,41>();

    // sb_[0][0].set<float>(987);

    // PLOGW << Instance(&testbuf).get<float,41>();

    // Instance(&testbuf).print();



}
