/*

    SMARTMAP (beta)

                        */

#include "engine.hpp"
#include "buffer.hpp"
#include "log.hpp"

namespace TEST {

    struct Struct;
    struct Buffer {

        std::vector<char> date;
        std::vector<Struct*> structs;

        Struct& addStruct(Struct s);

    };
    struct Struct {

        std::string name;

        uint32_t size_v = 0;

        bool is_striding = false;
        
        uint32_t size() { 

            if (is_striding) return nextFactor(size_v,16);
            
            return size_v; 
            
        }

        uint32_t stride() { return size()-size_v; }

        void striding(bool is_striding){ 

            this->is_striding = is_striding; 

        }

        bool striding() { return is_striding; }

        std::vector<Struct> members;

        void print(int tab = 0) {

            std::string str;
            for (int i = 0; i < tab; i++) str+="\t";
            str+=name;
            PLOGD << str;

            for (auto &m :members) print(tab+1);

        }

    };

    Struct& Buffer::addStruct(Struct s) {

        return s;

    }
 
}

int main() {
    
TEST::Buffer buffy;
auto s = buffy.addStruct(TEST::Struct{"test"});

s.print();
 

}