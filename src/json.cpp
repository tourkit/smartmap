#include "json.hpp"

#include "file.hpp"
#include "utils.hpp"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include <cstddef>
#include <regex>

JSON::JSON() : JSONVal(document) {  }

JSON::JSON(File* file) : JSON() { load(file); }

JSON::JSON(const char* data) : JSON() { load(data); }

JSON::~JSON() {  }

bool JSON::load(File* file) {

    this->file = file;

    return load(file->data.data());

}




bool JSON::load(const char* data) {

    loaded = false;

    rapidjson::ParseResult result = document.Parse(data);

    if (document.IsNull()) {

        PLOGE << rapidjson::GetParseError_En(result.Code()) << " @ " <<  result.Offset();

        PLOGV << data;

        return false;

    }

    loaded = true;

    return true;

}


void JSON::if_obj_in(std::string name, rapidjson::Value &in, std::function<void(rapidjson::Value::Member&)> cb) {

    if (!in.HasMember(name.c_str()) || !in[name.c_str()].IsObject()) return;

    for (auto &m : in[name.c_str()].GetObj()) cb(m);

}



std::string JSONVal::stringify() {


    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    value.Accept(writer);

    return std::regex_replace(buffer.GetString(), std::regex("[\r\n\t ]+"), " ");

}


  JSONVal::JSONVal(rapidjson::Value &value, std::string name, JSONVal*owner, JSONVal*parent) : value(value), name_v(name), owner(owner), parent(parent) {

    if (!owner)
        owner = this;

    if (value.IsArray()) {

        auto arr =  value.GetArray();

        for (auto &x : arr) 
            childrens.emplace_back(x, "", owner) ;

        return;
        
    }

    if (value.IsObject()) {

        auto obj =  value.GetObj();

        for (auto &x : obj) 
            if (x.name.IsString()) 
                childrens.emplace_back(x.value, x.name.GetString(), owner) ;

        return;
        
    }

}

JSONVal::~JSONVal() { }

JSONVal::JSONVal(const JSONVal& other) : value(other.value), owner(other.owner), parent(other.parent), childrens(other.childrens), name_v(other.name_v) {




}

JSONVal& JSONVal::find(std::function<bool(JSONVal&)> cb) {

    if (cb(*this))
        return *this;

    for (auto &entry : *this) {

        JSONVal& result = entry.find(cb);
        if (!(&result == &json_null))
            return result;
        
    }

    return json_null;

}

JSONVal& JSONVal::find(std::string name, std::string val) {

    return find([name,val](JSONVal& entry) { 

        for (auto x : split(name, "|")) 
            for (auto y : split(val, "|")) 
                if(lower(entry[name].str()) == val)
                return true; 
        
        return false; 

    });

}


JSONVal JSONVal::operator[](std::string name, bool warn) { 

    for (auto x : split(name, "|")) 

        if (value.IsObject())

            for (auto &v : value.GetObj()) 
                
                if (v.name.IsString() && x  == lower(v.name.GetString())) 

                    return JSONVal(value[x.c_str()], x, owner, this);


    if (warn)
        {PLOGW << "no " << name << " in " << this->name() << " (" << str() << ")";}
        
    return JSONVal(null_val);
    
}


JSONVal JSONVal::operator[](int id, bool warn) { 

    if (value.IsArray() && id < value.GetArray().Size()) return JSONVal(value.GetArray()[id], "", owner, this);
    
    if (warn)
        {PLOGW << "no " << id;}

    return JSONVal(null_val);
    
}

std::string JSONVal::name() { return name_v; }

bool JSONVal::isnum() {     
    
    if (value.IsFloat()) return value.IsFloat();

    if (value.IsInt()) return value.IsInt();
    
    return 0; 
}
bool JSONVal::isarr() {     
    
    if (value.IsArray()) return true;
    
    return false; 
}

bool JSONVal::isobj() {     
    
    if (value.IsObject()) return true;
    
    return false; 
}

size_t JSONVal::size() { 

    if (isarr()) return value.GetArray().Size();
    
    if (isobj()) return value.GetObj().MemberCount();
    
    return 0; 
    

}

bool JSONVal::b(bool def) {

    if (value.IsBool()) 
        return value.GetBool();
    if (value.IsInt()) 
        return value.GetInt();
    if (value.IsFloat()) 
        return value.GetFloat();
    if (value.IsString()) 
        return (!strcmp(value.GetString(),"true") + !strcmp(value.GetString(),"active")) ;

    return def;

}

float JSONVal::num(float def) { 
    
    if (value.IsFloat()) return value.GetFloat();

    if (value.IsInt()) return value.GetInt();
    
    return def; 
    
}

std::string JSONVal::str(std::string def) { 
    
    if (value.IsString()) return value.GetString();  
    
    return def; 
    
}
