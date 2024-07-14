#include "json.hpp"

#include "file.hpp"
#include "log.hpp"
#include "rapidjson/error/en.h"

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

    if (!result) PLOGE << rapidjson::GetParseError_En(result.Code()) << " @ " <<  result.Offset();

    if (document.IsNull()) {

        PLOGE << rapidjson::GetParseError_En(document.GetParseError());
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



  JSONVal::JSONVal(rapidjson::Value &value, std::string name) : value(value), name_v(name) {

    if (value.IsArray()) {

        auto arr =  value.GetArray();

        for (auto &x : arr) childrens.emplace_back(x) ;

        return;
        
    }

    if (value.IsObject()) {

        auto obj =  value.GetObj();

        for (auto &x : obj) if (x.name.IsString()) childrens.emplace_back(x.value, x.name.GetString()) ;

        return;
        
    }

}

JSONVal::~JSONVal() { }


JSONVal JSONVal::operator[](std::string name) { 


    if (!value.HasMember(name.c_str())) {
        PLOGW << "no " << name;
        return JSONVal(value);
    }
    
    return JSONVal(value[name.c_str()]);
    
}


JSONVal JSONVal::operator[](int id) { 

    if (value.IsArray() && id < value.GetArray().Size()) return JSONVal(value.GetArray()[id]);
    
    PLOGW << "no " << id;

    return JSONVal(value);
    
}


std::string JSONVal::name() { return name_v; }

std::string JSONVal::str() { 
    
    if (value.IsString()) return value.GetString();  
    
    if (value.IsFloat()) return std::to_string(value.GetFloat()); 

    if (value.IsInt()) return std::to_string(value.GetInt()); 
    
    return ""; 
    
}
