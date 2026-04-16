#include "struct.hpp"






#include <cstddef>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <vector>
#include <boost/type_index.hpp>



Struct_::Struct_(const char* n) { 
    if (n) 
        this->name_v = n; 
} 

    
Struct_::Field_::Field_( Struct type, const char* label, int q, float from, float to, float def) : quantity_v(q) {

    if (label)
        label_v = label;

    this->type(type);

    if (from || to || def)
        range(from, to, def); 
}


uint32_t Struct_::Field_::footprint_all() { return type_v->footprint() *  quantity_v ; }

void Struct_::Field_::trig(Event e){}    

void Struct_::Field_::type(Struct type) {

    type_v = type;
    

}

void Struct_::Field_::type(uint32_t id) {

    //type_v. = id;
    

}


void Struct_::Field_::range(float from, float to, float def) {

            if (!type_v->size()) 
                return;

            rangedef.resize(type_v->size()*3);
            memset(&rangedef[0],0,rangedef.size());

            //set(type_v->type_v, this->from(), from);
            //set(type_v->type_v, this->def(), def);
            //set(type_v->type_v, this->to(), to);
   
        }

char* Struct_::Field_::from() { return rangedef.size()?rangedef.data():nullptr; }

char* Struct_::Field_::to() { 
    
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

char* Struct_::Field_::def() { 

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


auto Struct_::Field_::begin() { return type_v->fields.begin(); }
auto Struct_::Field_::end() { return type_v->fields.end(); }
    
    Struct_::Field* Struct_::operator[] (std::string q) {

        for (auto& e:fields) 
            if (!strcmp(q.c_str(), e->label_v.c_str()))
                return &e;
        
        return nullptr;
    }       
    uint32_t Struct_::stride() { return 0 ; }

  
    uint32_t Struct_::footprint() { return size() + stride() ; } // missleading not only first instance footprint if array

 
    // CRUD

    Struct_::Field Struct_::add(Struct type, const char* label, int quantity, float from, float to, float def) {

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

    // OBSERVERS


    void Struct_::addObserver(Struct m, int q) {

        if (observers.find(m) == observers.end())
            observers[m] = q;
        else
            observers[m]+=q;
    }

    void Struct_::removeObserver(Struct m, int q) {

        if (observers.find(m) == observers.end())
            std::cout << "\nerore\n\n"; // FIXME :: WTF
        
        observers[m]-=q;
        
        if (observers[m] < 1)   
            observers.erase(m);

    }
    
    std::set<Struct> Struct_::getTop() {

        std::set<Struct> out;

        if (!observers.size()) 
            out.insert(shared_from_this());
        
        for (auto observer : observers)     
            for (auto x :   observer.first->getTop())
                out.insert(x);

        return out;

    }
  

    auto Struct_::name() {

        if (name_v.empty())

            return boost::typeindex::type_index(type_v).pretty_name();

        return name_v;

    }




Definition Struct_::add(const TypeIndex& type, const char* name, int quantity, float from, float to, float def) {

    return add(reg->find(type), name, quantity, from, to, def);
}
uint32_t Struct_::size() {

    size_t size_v = 0;

    for (auto def : fields) 
        size_v += def->footprint_all(); 

    return size_v;
}

std::string Struct_::json() {


    std::string out = "{";

    for (auto f : fields) {
    
            //f
    }

    out = out + "}";
    return out+"";
}

std::string Struct_::serializeSafe( int depth) 
{
    using namespace rapidjson;

    if (depth > 10) {
        Document err(kObjectType);
        auto& alloc = err.GetAllocator();
        err.AddMember("error", "max depth reached", alloc);

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        err.Accept(writer);
        return buffer.GetString();
    }

    Document d(kObjectType);
    Document::AllocatorType& alloc = d.GetAllocator();

    d.AddMember("name", Value(name_v.c_str(), alloc), alloc);

    Value fieldsArr(kArrayType);

    for (const auto& f : fields) {
        Value obj(kObjectType);

        obj.AddMember("label",
                      Value(f->label_v.c_str(), alloc),
                      alloc);

        // parse nested type JSON string
        Document typeDoc;
        typeDoc.Parse(f->type_v->serializeSafe(depth + 1).c_str());

        Value typeVal;
        typeVal.CopyFrom(typeDoc, alloc);

        obj.AddMember("type", typeVal, alloc);

        if (f->quantity_v > 1) {
            obj.AddMember("quantity", f->quantity_v, alloc);
        }

        fieldsArr.PushBack(obj, alloc);
    }

    d.AddMember("fields", fieldsArr, alloc);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    return buffer.GetString();
}


std::string Register_::serializeSafe(int depth)
{
    rapidjson::Document doc;
    doc.SetArray();
    auto& allocator = doc.GetAllocator();

    if (depth > 10) {
        rapidjson::Value err(rapidjson::kObjectType);
        err.AddMember("error", "max depth reached", allocator);
        doc.PushBack(err, allocator);

    } else {

        for (const auto& s : structtypes) {

            rapidjson::Value obj(rapidjson::kObjectType);

            obj.AddMember(
                "name",
                rapidjson::Value(s->name_v.c_str(), allocator),
                allocator
            );

            if (!s->fields.empty()) {

                rapidjson::Value fields(rapidjson::kArrayType);

                for (const auto& f : s->fields) {

                    rapidjson::Value fieldObj(rapidjson::kObjectType);

                    fieldObj.AddMember(
                        "label",
                        rapidjson::Value(f->label_v.c_str(), allocator),
                        allocator
                    );

                    fieldObj.AddMember(
                        "type",
                        rapidjson::Value(f->type_v->name_v.c_str(), allocator),
                        allocator
                    );

                    if (f->quantity_v > 1) {
                        fieldObj.AddMember(
                            "quantity",
                            f->quantity_v,
                            allocator
                        );
                    }

                    fields.PushBack(fieldObj, allocator);
                }

                obj.AddMember("fields", fields, allocator);
            }

            doc.PushBack(obj, allocator);
        }
    }

    // 👉 Convert to string
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    return buffer.GetString();
}