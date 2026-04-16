
#include "string.hpp"
#include <boost/type_index.hpp>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <map>


struct Arena {

    std::vector<char> data;

    struct Element {
        
        size_t offset = 0;
        size_t length = 0;

    };

    std::unordered_map<uint64_t, Element> elements;

    std::vector<std::pair<size_t, size_t>> free_list; 

    uint64_t next_id = 0;

    size_t allocate(size_t len) {
        
        for (auto it = free_list.begin(); it != free_list.end(); ++it) {
            if (it->second >= len) {
                size_t offset = it->first;

                if (it->second > len) {
                    it->first += len;
                    it->second -= len;
                } else {
                    free_list.erase(it);
                }

                return offset;
            }
        }

        
        size_t offset = data.size();
        data.resize(offset + len);
        return offset;
    }

    uint64_t add(const char* src, size_t len) {

        uint64_t id = next_id++;

        size_t offset = allocate(len);

        std::copy(src, src + len, data.data() + offset);

        elements[id] = { offset, len };

        return id;

    }

    void free(size_t offset, size_t len) {

        free_list.push_back({ offset, len });

    }

    void set(uint64_t id, const char* src, size_t len) {

        auto it = elements.find(id);
        if (it == elements.end()) return;

        auto& e = it->second;

        if (len <= e.length) {
            // overwrite in-place
            std::copy(src, src + len, data.data() + e.offset);
            e.length = len;
            return;
        }

        // sinon: libérer ancien + réallouer
        free(e.offset, e.length);

        size_t offset = allocate(len);
        std::copy(src, src + len, data.data() + offset);

        e.offset = offset;
        e.length = len;
    }

    void remove(uint64_t id) {

        auto it = elements.find(id);
        if (it == elements.end()) return;

        free(it->second.offset, it->second.length);
        elements.erase(it);
    }
    
    std::string str(uint64_t id) { 
        
        auto it = elements.find(id);
        if (it == elements.end()) return "";
        auto& e = it->second;
        
        return std::string(data.data() + e.offset, e.length); 
        
    };

};

template <typename T>
struct Version {
    
    static inline uint64_t version_count = 0;

    static uint64_t change() {
        
            version_count++;
        
            return version_count;
    }

};

struct ContiguousOrderedBuffer {

    using Version = Version<ContiguousOrderedBuffer>;

    std::vector<char> data;
    
    struct Element {uint64_t offset,length; };
    std::unordered_map<uint64_t, Element> elements;
    uint32_t fixed_size = 0 ;

    // CRUD

    static inline uint64_t id_count = 0;
    uint64_t create(const char* data = nullptr, uint32_t len = 0) {

        uint64_t id = id_count++;

        auto& e = elements[id];
        
        e.offset = this->data.size();

        if (data)
            set(id, data, len);

        else
            Version::change();
        
        return id;

    }

    
    void set(uint64_t id, const char* data, uint32_t length = 0) {

        auto it = elements.find(id);
        if (it == elements.end()) return;
        auto& e = it->second;
        
        if (!length) {
            
            if (fixed_size)
            length = fixed_size;
            else
            length = strlen(data);
        }

        resize(id, length);
    
        memcpy(&this->data[e.offset], data, length);

        Version::change();
        
    }
    
    void remove(uint64_t id) {

        auto it = elements.find(id);
        if (it == elements.end()) return;
        auto& e = it->second;
            
        resize(id, 0);
        
        elements.erase(id);

        Version::change();
            
    }

    // HELPERS

    std::string_view strview(uint64_t id) { 
            
        auto it = elements.find(id);
        if (it == elements.end()) return "";
        auto& e = it->second;
        
        return std::string_view(data.data() + e.offset, e.length); 
        
    };
    
    std::string str(uint64_t id) { 
        
        auto it = elements.find(id);
        if (it == elements.end()) return "";
        auto& e = it->second;
        
        return std::string(data.data() + e.offset, e.length); 
        
    };
private:
    void resize(uint64_t id, size_t length){

        auto it = elements.find(id);
        if (it == elements.end()) return;
        auto& e = it->second;

        int diff =  length-e.length;

        auto old_size = data.size();

        if (length && old_size+diff>data.capacity()) 
            data.reserve(std::max(old_size,length)*2);

        data.resize(old_size+diff);

        auto old_end = e.offset+e.length;

        memmove(&data[old_end+diff], &data[old_end], old_size-old_end);

        e.length = length;

        for(auto& x : elements)
            if (x.second.offset>e.offset)   
                x.second.offset += diff;
    }

};


static uint64_t random64() {

    std::random_device rd; 
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;

    return dist(gen);

}
struct Op {
    enum Type { Create, Set, Resize, Remove };
    uint64_t id;
    std::string data;
    uint32_t size;
};

struct TypeRegistry {
    
    using StringID = uint64_t;

    Arena name_strings;
    Arena label_strings;
    
    struct BaseDefinition {
        
        StringID name;
        
        int size = 0; 

        BaseDefinition(const BaseDefinition&) = delete;

    };
    
    struct StructDefinition : BaseDefinition {

        struct Field {
            
            StringID label;
            
            BaseDefinition type;

            uint32_t quantity = 1;

        };

        std::vector<Field> fields;
        
    };
    enum Kind { Base,Struct };
    
    struct TypeHandle;

    static inline std::map<uint64_t,TypeHandle> list;
    static inline std::vector<BaseDefinition> bases;
    static inline std::vector<StructDefinition> structs;

    struct TypeHandle {
        
        Kind kind;
        
        uint64_t id = 0;

        StructDefinition* strct() { 

            if (list[id].kind == Kind::Struct) 
                return &structs[list[id].id];
            
            return nullptr;
        }

        BaseDefinition* base() { 

            if (list[id].kind == Kind::Base) 
                return &bases[list[id].id];

            return strct();
            
        }
        
    };
    
    template<typename T>
    TypeHandle Type() {
    
        static uint32_t T_id = 0;
        
        if (!T_id) {
            
            std::string pname = boost::typeindex::type_id<T>().pretty_name();

            auto n = TypeRegistry::Type(pname.c_str());

            n.base()->size = sizeof(T);

            T_id = n.id;
            
            return n;

        }
    
        return list[T_id];
    
    }
    
    TypeHandle Type(const char* name, uint32_t key = 0) {

        for (auto& e : TypeRegistry::list) {

            if (!strcmp(name_strings.str(e.second.base()->name).c_str(), name))

                return e.second;
            
        }

        if (!key)
            while ((key = random64()))
                if (!TypeRegistry::list.contains(key))
                    break;

        // conment gerer l insertions dans bases et structs sekolon le kind ?  /// ///// <<_____----------

         
        auto* x = TypeHandle{TypeRegistry::list[key].second}.base();

        x->name = name_strings.add(name);

        return TypeHandle{key};

    }
    
    
};

