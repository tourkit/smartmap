

#include <cstddef>
#include <cstdint>
#include <iostream>


#include "nlohmann/json_fwd.hpp"
#include "vendors/nlohmann/json.hpp"


// WTF is a  Instance ?  
// 
// 


/*



why Type is not a Struct ?


struct TypeIndex : boost::typeindex::type_index ;
template <typename T>
struct TYPE : TypeIndex {}

struct Struct { // fais struct aussi ? pk a un nom ?

    struct Field {} // wtf ?
    struct Instance {
        struct Element {} // wtf ?
    }  

} 
struct Data : Struct {}  // typé(int,string,..) aka non struct
struct Buffer : Struct {} // stored
struct Register {} //



diff member / def ? watiz def ?


faut creer un struct pour int, float et aussi monStruct etc . pur utiliser dans Field->type

*/

#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <sys/types.h>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <boost/type_index.hpp>

#define SHAREDSTRUCT_(__NAME__) STRUCT_(__NAME__) : std::enable_shared_from_this<__NAME__##_>

#define STRUCT_(__NAME__) struct __NAME__##_ ;\
struct __NAME__ : std::shared_ptr<__NAME__##_> {\
\
    __NAME__(std::shared_ptr<__NAME__##_> ptr = nullptr) : std::shared_ptr<__NAME__##_>(ptr) {}\
\
    template <typename... Args>\
    static __NAME__ Create(Args&&... args) { return (__NAME__)std::make_shared<__NAME__##_>(std::forward<Args>(args)...); } \
};\
struct __NAME__##_



using boost::typeindex::type_index;

struct TypeIndex : boost::typeindex::type_index {

    using boost::typeindex::type_index::type_index;

    static inline std::map<TypeIndex, int> sizes;
    static int size(TypeIndex t, int def = 0) {

        auto it = TypeIndex::sizes.find(t);
        if (it == TypeIndex::sizes.end())
            TypeIndex::sizes[t] = def;
            
        return it->second;

    }

};

template <typename T>
struct TYPE : TypeIndex { 
    
    TYPE() : TypeIndex(typeid(T)) { 
        
        TypeIndex::size(typeid(T), sizeof(T));

    }
};

SHAREDSTRUCT_ (Struct) {

    std::string name_v;

    int size_v = 0; //  passively maintained by Buffer::post() // strange weird shoul be in buffer ? maybe that static map ?

    Struct_(const char* n = nullptr) { 
        if (n) 
            this->name_v = n; 
    } 

    enum Event { PRE};

    SHAREDSTRUCT_(Field) {

        std::string label_v;
        
        Struct type_v;

        int quantity_v;
    
        Field clone_v;
        
        Field_( Struct type, const char* label = nullptr, int q = 1, float from = 0, float to = 0, float def = 0) : quantity_v(q) {
    
            if (label)
                label_v = label;

            this->type(type);

            if (from || to || def)
                range(from, to, def); 
        }

        std::vector<char> rangedef; 
        std::vector<char> temprangedef; 

        uint32_t footprint_all() { return type_v->footprint() *  quantity_v ; }

        void trig(Event e){}    
        
        void type(Struct type) {

            type_v = type;
            

        }

        void type(uint32_t id) {

            //type_v. = id;
            

        }


        bool quantity(int q);

        void range(float from, float to, float def) {

            if (!type_v->size()) 
                return;

            rangedef.resize(type_v->size()*3);
            memset(&rangedef[0],0,rangedef.size());

            //set(type_v->type_v, this->from(), from);
            //set(type_v->type_v, this->def(), def);
            //set(type_v->type_v, this->to(), to);
   
        }

        char* from() { return rangedef.size()?rangedef.data():nullptr; }

        char* to() { 
            
            if (rangedef.size())
                return rangedef.data()+(type_v->size());
            
            static uint64_t ui64 = 0xffffffffffffffffu;
            static int64_t i64   = 0xffffffffffffffff;
            static double f32    = 0xfffffffffffff;

            if (type_v->type_v == typeid(float)) return (char*)&f32;
            if (type_v->type_v == typeid(double)) return (char*)&f32;
            if (type_v->type_v == typeid(uint64_t)) return (char*)&ui64;
            if (type_v->type_v == typeid(uint32_t)) return (char*)&ui64;            
            if (type_v->type_v == typeid(uint16_t)) return (char*)&ui64;
            if (type_v->type_v == typeid(uint8_t)) return (char*)&ui64;
            if (type_v->type_v == typeid(int64_t)) return (char*)&i64;
            if (type_v->type_v == typeid(int32_t)) return (char*)&i64;            
            if (type_v->type_v == typeid(int16_t)) return (char*)&i64;
            if (type_v->type_v == typeid(int8_t)) return (char*)&i64;

            return nullptr;

        }

        char* def() { 

            if (rangedef.size()) return rangedef.data()+(type_v->size()*2);
            
            if (type_v->type_v == typeid(Field_)) {

                if (!type_v->size())
                    return nullptr;

                temprangedef.resize(type_v->size());

                int offset = 0;

                for (auto def : type_v->fields) 

                    for (int i = 0; i < def->quantity_v; i++) {

                        memcpy(temprangedef.data()+offset, def->def(), def->type_v->size());    

                        offset += def->type_v->size();

                }

                return temprangedef.data();

            }

            return nullptr; 
            
        }


        //template <typename T, typename... Args>
        //Field add(Args&&... args) { return type_v->add<T>(std::forward<Args>(args)...); } 
        //template <typename... Args>
        //Field add(Args&&... args) { return type_v->add(std::forward<Args>(args)...); } 
        //template <typename T, int q = 1, typename... Args>
        //Field add(Args&&... args) { return type_v->add<T,q>(std::forward<Args>(args)...); } 


        auto begin() { return type_v->fields.begin(); }
        auto end() { return type_v->fields.end(); }
    
    };

    std::vector<Field> fields;
     static inline Field nofield(TYPE<Field>);
    
    Field* operator[] (std::string q) {

        for (auto& e:fields) 
            if (!strcmp(q.c_str(), e->label_v.c_str()))
                return &e;
        
        return nullptr;
    }       
    uint32_t stride() { return 0 ; }

    virtual uint32_t size(); // dynamicly calculated

    uint32_t footprint() { return size() + stride() ; } // missleading not only first instance footprint if array

    TypeIndex type_v = typeid(*this);

    // CRUD

    Field add(Struct type, const char* label = nullptr, int quantity = 1, float from = 0, float to = 0, float def = 0) {

        type->addObserver(shared_from_this(),quantity);

        auto observers = getTop();

        int compoffset = footprint(); // or pos <- what is this com?
        
        for (auto x :  observers)
            x->pre(shared_from_this(), compoffset);
        
        auto definition = Field::Create(type, label, quantity, from, to, def);

        int compsize = definition->type_v->footprint();

        auto defval = definition->def();

        // std::string cout = this->label+ "[" +std::to_string(footprint()) + "] add  " + definition->label + "(" + definition->type_v->label + ":" + std::to_string(compsize);
        // if (definition->quantity_v > 1)
        //     cout += "*" + std::to_string(definition->quantity_v)  + ":" + std::to_string(definition->footprint_all());
        // cout += ")";
        // std::cout << cout << std::endl;

        fields.emplace_back(definition);

        for (auto x :   observers)
            x->post(definition->quantity_v,compsize, defval, 0);

        return definition;

    }

    Field add(const TypeIndex& type, const char* label, int quantity, float from, float to, float def);

    template <typename T, int q = 1>
    Field add(const char* label, float from = 0, float to = 0, float def = 0) {

        return add(TYPE<T>(), label, q, from, to, def);
        
    }
    


    // OBSERVERS

    std::map<Struct,int> observers;

    void addObserver(Struct m, int q = 1) {

        if (observers.find(m) == observers.end())
            observers[m] = q;
        else
            observers[m]+=q;
    }

    void removeObserver(Struct m, int q = 1) {

        if (observers.find(m) == observers.end())
            std::cout << "\nerore\n\n"; // FIXME :: WTF
        
        observers[m]-=q;
        
        if (observers[m] < 1)   
            observers.erase(m);

    }
    
    std::set<Struct> getTop() {

        std::set<Struct> out;

        if (!observers.size()) 
            out.insert(shared_from_this());
        
        for (auto observer : observers)     
            for (auto x :   observer.first->getTop())
                out.insert(x);

        return out;

    }
  
    virtual void pre(Struct changing, int compoffset) {}

    virtual void post(int diff, int compsize, char* def, int q) {}

    auto name() {

        if (name_v.empty())

            return boost::typeindex::type_index(type_v).pretty_name();

        return name_v;

    }

    std::string json() ;

    nlohmann::ordered_json serializeSafe(int depth = 0);
    
};

using Definition = Struct_::Field;

STRUCT_(Data) : Struct_ {

    Data_(const TypeIndex& type, const char* name)  { 

        type_v = type; 

        this->name_v = name ? name : type_v.pretty_name();
        
        size_v = TypeIndex::size(type_v); 

    }

    uint32_t size() override {
        return TypeIndex::size(type_v);
    }

};

SHAREDSTRUCT_(Register) {

    std::set<Data> datatypes;
    std::set<Struct> structtypes;

    template <typename T>
    Data create(const char* name = nullptr) {

        return create(TYPE<T>(), name?name:TYPE<T>().pretty_name());
        
    }

    // static inline Register current;

    Data create(const TypeIndex& type, const char* name = nullptr) {

        auto d = Data::Create(type, name);

        // std::cout << "create " << d->quantity_v << " " << d->name << (d->quantity_v>1?"s":"")<<  " " << Type(type).name() << " - " << Type(type).size() << "\n";

        datatypes.insert(d);

        return d;
        
    }

    template <typename T>
    Data create() {

        return create<T>("");
        
    }

    Struct create(const char* name) {

        auto s = Struct::Create();

        s->name_v = name;

        // std::cout << "create " << s->quantity_v << " " << s->name << (s->quantity_v>1?"s":"")<<  ""  << "\n";

        structtypes.insert(s);

        return s;
        
    }
 
    Struct find(const TypeIndex& type) {

        // type.type_info().name()
        for (const auto& x : datatypes) 
            if (x.get()->type_v == type) 
                return x->shared_from_this();

        return create(type)->shared_from_this();
        
    }
 
    template <typename T>
    Struct find() {
        return find(TYPE<T>());
    }

    Struct changing;


    nlohmann::ordered_json serializeSafe(int depth = 0);
};


static Register reg = Register::Create();

Definition Struct_::add(const TypeIndex& type, const char* name, int quantity, float from, float to, float def) {

    return add(reg->find(type), name, quantity, from, to, def);
}
uint32_t Struct_::size() {
    
    size_t size_v = 0;

    for (auto def : fields) 
        size_v += def->footprint_all(); 

    return size_v;
}

#include <set>

#include <algorithm>

//ranger par ordre de longeur

//template <typename T>
//struct Pool {

//        std::vector<T*> list;

//};

struct String {

    static inline std::vector<char> data; // could be 2 (shortstr and longtxt)
    static inline std::vector<String*> pool;

    static void defrag() {

        // create new order list

        // create new empty buffer

        // populate 

        // move new to old

    }

    ////////////////////////////////////////////////////

    uint32_t offset;
    uint32_t length = 0;

    String(const char* value = nullptr) {

        pool.push_back(this);
        offset = data.size();

        if (value)
            set(value);

    }

    String(const String& other)  : offset(other.offset), length(other.length) { }

    ~String() {

        resize(0);

        pool.erase(std::remove(pool.begin(), pool.end(), this), pool.end());
        
    }

    bool set(const char* value) {

        auto len = strlen(value);

        if (len > length)
            resize(len);
        else
            length = len;
            
        memmove(&data[offset], value, len);

        return 1;

    }

    char* mut() { return &data[offset]; }

    auto view() { return std::string_view(data.data() + offset, length); }

    String string() { return String(*this); } 

private:

    void resize(size_t length) {
        
        int diff =  length-this->length;

        auto old_size = data.size();

        if (length && old_size+diff>data.capacity()) 
            data.reserve(std::max(old_size,length)*2);

        data.resize(old_size+diff);

        auto end = offset+this->length;

        memmove(&data[end]+diff, &data[end], old_size-end);

        this->length = length;

        for(auto e : pool)
            if (e->offset>offset)   
                e->offset += diff;

    }

};

template <typename T>
struct AoS {

    std::vector<T> data;

    struct Element {};

};


std::string Struct_::json() {


    std::string out = "{";

    for (auto f : fields) {
    
            //f
    }

    out = out + "}";
    return out+"";
}

nlohmann::ordered_json Struct_::serializeSafe( int depth) {
    if (depth > 10) {
        return { {"error", "max depth reached"} };
    }

    nlohmann::ordered_json j;

    j["name"] = name_v;

    if (fields.size()) {

        j["fields"] = nlohmann::json::array();
        
        for (const auto& f : fields) {

            j["fields"].push_back({
                {"label", f->label_v},
                {"type", f->type_v->serializeSafe(depth + 1)}
            });

            if (f->quantity_v > 1)      
                j["fields"].back()["quantity"] = f->quantity_v;

        }
    }


    return j;
}

nlohmann::ordered_json Register_::serializeSafe( int depth) {

    if (depth > 10) {
        return { {"error", "max depth reached"} };
    }

    nlohmann::ordered_json j;
    
    j = nlohmann::json::array();

    for (const auto& s : structtypes) {

        j.push_back({
            {"name", s->name_v},
        });

        if (s->fields.size()) {

            j.back()["fields"] = nlohmann::json::array();

            int i = 0;
            for (const auto& f : s->fields){
            
            j.back()["fields"].push_back({
                {"label", f->label_v},
                {"type", f->type_v->name_v}
            });
                
                
                if (f->quantity_v > 1)      
                    j.back()["fields"].back()["quantity"] = f->quantity_v;

                i++;
                
            }
        }
    };

    return j;
}


#include <App.h>
#include <iostream>

nlohmann::ordered_json json_message(const char* type, nlohmann::ordered_json body){

    nlohmann::ordered_json message;
    message["type"] = type;
    message["body"] = body;


    return message;

}
int main() {


    auto coord3d = reg->create("3D");
    coord3d->add<float>("x",-1,1,0);
    coord3d->add<float>("y",-1,1,0);
    coord3d->add<float>("z",-1,1,0);


    auto objet_struct = reg->create("Objet");

    objet_struct->add(coord3d,"coord");
    objet_struct->add<int,4>("RGBA",0,1,1);

    auto piece_struct = Struct::Create ("Piece");

        std::cout << reg->serializeSafe() << std::endl;
    struct SringIndex{};
    piece_struct->add<SringIndex>("nom"); 
    //piece_struct.add(objet_struct, "table",);
    auto chaises = piece_struct->add(objet_struct, "chaises", 4);
    piece_struct->add<int>("ouverte");




    uWS::App()


    .any("/*", [](auto* res, auto* req) {
        res->writeStatus("200 Found");
        res->end();

    std::string url = std::string(req->getUrl());
    std::string method = std::string(req->getMethod());
    std::transform(method.begin(), method.end(), method.begin(), ::toupper);
    std::string body;

    res->onData([res, method, url, body = std::move(body)](std::string_view chunk, bool isLast) mutable {
        body.append(chunk);

        if (isLast) 
            std::cout << method << " " << url << (strlen(body.c_str()) ? " -> " : "")  << body << std::endl;
            
        
    });
})



    .ws<std::string>("/*", {
        .open = [](auto* ws) {
            std::cout << "Client connected\n";
        },

        .message = [](auto* ws, std::string_view msg, uWS::OpCode op) {
            std::string_view m = msg;

            if (m == "registre") {

                auto message = json_message(std::string(m).c_str(),reg->serializeSafe()).dump();
                std::cout << message << std::endl;
                
                ws->send(message, uWS::OpCode::TEXT);
            }else if (m == "files") {

                nlohmann::ordered_json json;

                json["type"] = "files";

                json["body"] = json["body"] = nlohmann::json::array({{ {"path", "config.json"} },{ {"path", "layout.json"} } });

                std::cout << json << std::endl;

                ws->send(json.dump(), uWS::OpCode::TEXT);

            }
        },

        .close = [](auto*, int, std::string_view) {
            std::cout << "Client disconnected\n";
        }


    })
    .listen(1337, [](auto* token) {
        if (token) {
            std::cout << "WebSocket running on ws://localhost:1337\n";
        }
    })
    .run();

    //(*objet_struct)["nom"] // find field by name

    //nom_objet.name("piece"); // change name to "piece"
    //nom_objet.name(); // return "piece"

    //objet_struct[2].type(); // return RGBA (3rd field) type "int"
    //objet_struct[2].type<float>(); //  set it to float()

    //objet_struct[2].destroy();
    //object_struct.count() // return 2;


}



